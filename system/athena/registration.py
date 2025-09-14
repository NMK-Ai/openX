#!/usr/bin/env python3
import time
import json
import requests
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.system.hardware import HARDWARE
from openpilot.common.swaglog import cloudlog

# 未注册设备时的默认 ID
UNREGISTERED_DONGLE_ID = "UnregisteredDevice"
# 注册服务器地址
REGISTER_URL = "https://mr-one.cn/v2/register.php"


def is_registered_device() -> bool:
    """
    判断当前设备是否已经注册
    如果 Params 中保存的 DongleId 为空或者等于默认未注册 ID，
    则说明设备未注册
    """
    dongle = Params().get("DongleId")
    return dongle not in (None, UNREGISTERED_DONGLE_ID)


def register(show_spinner=True) -> str:
    """
    设备注册流程：
    1. 如果已有 dongle_id，直接返回，不再重复注册
    2. 如果没有，循环向服务器注册，直到成功
    """
    params = Params()

    # ✅ 已注册过，直接返回
    if is_registered_device():
        dongle_id = params.get("DongleId")
        cloudlog.info(f"Device already registered, dongle_id={dongle_id}")
        if show_spinner:
            spinner = Spinner()
            spinner.update(f"Device already registered, dongle_id={dongle_id}")
            time.sleep(2)
            spinner.close()
        return dongle_id

    # ❌ 未注册，进入注册流程
    spinner = Spinner() if show_spinner else None

    while True:  # 无限循环，直到注册成功
        serial = HARDWARE.get_serial()
        if spinner:
            spinner.update(f"Registering device... serial={serial}")

        try:
            # 向服务器发起 POST 请求
            resp = requests.post(
                REGISTER_URL,
                json={"serial": serial},
                timeout=5,
                verify=False  # 禁用 SSL 证书验证（自签证书时使用）
            )

            # 解析返回结果
            try:
                dongleauth = resp.json()
            except json.JSONDecodeError:
                dongleauth = {}
                #cloudlog.warning("Registration server returned invalid JSON")

            if dongleauth.get("status") == "ok" and dongleauth.get("dongle_id"):
                # 注册成功
                dongle_id = dongleauth["dongle_id"]
                params.put("DongleId", dongle_id)
                #cloudlog.info(f"Registration successful, dongle_id={dongle_id}")
                if spinner:
                    spinner.close()
                return dongle_id
            else:
                # 注册失败，打印错误
                #msg = dongleauth.get("message", "Unknown error")
                #cloudlog.warning(f"Registration failed: {msg}")
                if spinner:
                    spinner.update(f"Registering device... serial={serial}")

        except requests.exceptions.RequestException as e:
            # 网络请求异常
            #cloudlog.exception("Registration request failed")
            if spinner:
                spinner.update(f"Network error: {e}, retrying...")

        # 等待 5 秒后重试
        time.sleep(5)


if __name__ == "__main__":
    dongle_id = register(show_spinner=True)
    print(f"Registration complete, dongle_id={dongle_id}")
