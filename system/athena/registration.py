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

        spinner = None
        if show_spinner:
            spinner = Spinner()
            spinner.update("registering device")

        backoff = 0
        start_time = time.monotonic()
        while True:
            try:
                resp = requests.post(
                    "https://mr-one.cn/v2/register.php",  # 你的服务器地址
                    json={"serial": serial},              # 发送 JSON
                    timeout=5,
                    #verify=False  # 如果是自签名证书，可关闭验证
                )

                cloudlog.info(f"HTTP状态码: {resp.status_code}")
                try:
                    dongleauth = resp.json()
                    cloudlog.info(f"服务器返回 JSON: {dongleauth}")
                except json.JSONDecodeError:
                    cloudlog.warning("注册服务器返回非法 JSON")
                    dongleauth = {}

                # 取 dongle_id
                dongle_id = dongleauth.get("dongle_id")
                if not dongle_id or dongleauth.get("status") != "ok":
                    dongle_id = UNREGISTERED_DONGLE_ID

                break

            except Exception:
                cloudlog.exception("注册请求失败，重试中")
                backoff = min(backoff + 1, 15)
                time.sleep(backoff)

            # 超时保护
            if time.monotonic() - start_time > 60:
                dongle_id = UNREGISTERED_DONGLE_ID
                cloudlog.warning("注册超时，使用默认 UnregisteredDevice")
                break

            if show_spinner and spinner:
                spinner.update(f"registering device - serial: {serial}, backoff={backoff}s")

        if show_spinner and spinner:
            spinner.close()

    if dongle_id:
        params.put("DongleId", dongle_id)
        cloudlog.info(f"注册完成，dongle_id={dongle_id}")

    return dongle_id

if __name__ == "__main__":
    print(register(show_spinner=True))
