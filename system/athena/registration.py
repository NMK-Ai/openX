#!/usr/bin/env python3
import time
import json
import requests
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.system.hardware import HARDWARE
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"

def is_registered_device() -> bool:
    dongle = Params().get("DongleId")
    return dongle not in (None, UNREGISTERED_DONGLE_ID)

def register(show_spinner=False) -> str | None:
    """
    使用自建 PHP 注册服务器的注册流程
    服务器返回 JSON: {"status":"ok","dongle_id":"xxxx"}
    """
    params = Params()
    dongle_id: str | None = params.get("DongleId")

    if dongle_id is None:
        serial = HARDWARE.get_serial()
        cloudlog.info(f"尝试向自建服务器注册设备 serial={serial}")

        spinner = Spinner() if show_spinner else None

        while True:
            try:
                resp = requests.post(
                    "https://mr-one.cn/v2/register.php",
                    json={"serial": serial},
                    timeout=5,
                    verify=False  # 如果是自签名证书，可关闭验证
                )

                cloudlog.info(f"HTTP状态码: {resp.status_code}")
                try:
                    dongleauth = resp.json()
                    cloudlog.info(f"服务器返回 JSON: {dongleauth}")
                except json.JSONDecodeError:
                    cloudlog.warning("注册服务器返回非法 JSON")
                    dongleauth = {}

                dongle_id = dongleauth.get("dongle_id")
                status = dongleauth.get("status")

                if not dongle_id or status != "ok":
                    msg = f"注册失败，设备未在白名单或返回错误状态: {dongleauth}"
                    cloudlog.warning(msg)
                    if spinner:
                        spinner.update(msg)
                    # 卡住，不再尝试
                    while True:
                        time.sleep(1)
                else:
                    # 注册成功
                    break

            except Exception as e:
                msg = f"注册请求异常: {e}"
                cloudlog.exception(msg)
                if spinner:
                    spinner.update(msg)
                # 卡住，不再尝试
                while True:
                    time.sleep(1)

        if spinner:
            spinner.close()

        params.put("DongleId", dongle_id)
        cloudlog.info(f"注册完成，dongle_id={dongle_id}")

    return dongle_id

if __name__ == "__main__":
    print(register(show_spinner=True))
