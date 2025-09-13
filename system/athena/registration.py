#!/usr/bin/env python3
import time
import json
import requests
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.system.hardware import HARDWARE
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"
REGISTER_URL = "https://mr-one.cn/v2/register.php"  # 替换成你的注册服务器地址

def is_registered_device() -> bool:
    dongle = Params().get("DongleId")
    return dongle not in (None, UNREGISTERED_DONGLE_ID)

def register(show_spinner=True) -> str:
    """
    注册设备，失败会一直卡住并显示错误信息。
    成功后立即写入 Params，保证 openpilot 启动。
    """
    params = Params()
    spinner = Spinner() if show_spinner else None

    while True:
        serial = HARDWARE.get_serial()
        if spinner:
            spinner.update(f"注册中... serial={serial}")

        try:
            resp = requests.post(
                REGISTER_URL,
                json={"serial": serial},
                timeout=5,
                verify=False  # 自签名证书可关闭验证
            )

            cloudlog.info(f"HTTP状态码: {resp.status_code}")

            try:
                dongleauth = resp.json()
            except json.JSONDecodeError:
                dongleauth = {}
                cloudlog.warning("注册服务器返回非法 JSON")

            cloudlog.info(f"服务器返回 JSON: {dongleauth}")

            if dongleauth.get("status") == "ok" and dongleauth.get("dongle_id"):
                dongle_id = dongleauth["dongle_id"]
                params.put("DongleId", dongle_id)
                cloudlog.info(f"注册成功，dongle_id={dongle_id}")
                if spinner:
                    spinner.close()
                return dongle_id
            else:
                msg = dongleauth.get("message", "未知错误")
                cloudlog.warning(f"注册失败: {msg}")
                if spinner:
                    spinner.update(f"注册失败: {msg}, 正在重试...")

        except requests.exceptions.RequestException as e:
            cloudlog.exception("注册请求失败")
            if spinner:
                spinner.update(f"网络错误: {e}, 正在重试...")

        # 每 5 秒重试一次
        time.sleep(5)


if __name__ == "__main__":
    dongle_id = register(show_spinner=True)
    print(f"注册完成，dongle_id={dongle_id}")
