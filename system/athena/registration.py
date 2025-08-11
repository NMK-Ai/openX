#!/usr/bin/env python3
import time
import json
import requests
from pathlib import Path
from datetime import datetime
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.selfdrive.selfdrived.alertmanager import set_offroad_alert
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.hardware.hw import Paths
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"

# 你的注册服务器地址
REGISTER_SERVER = "https://mr-one.cn/register.php"
# 如果 API 需要 key，可以加在这里
API_KEY = "my_secret_key"

def is_registered_device() -> bool:
    dongle = Params().get("DongleId", encoding='utf-8')
    return dongle not in (None, UNREGISTERED_DONGLE_ID)

def register(show_spinner=False) -> str | None:
    params = Params()

    dongle_id: str | None = params.get("DongleId", encoding='utf8')
    if dongle_id is None and Path(Paths.persist_root() + "/comma/dongle_id").is_file():
        with open(Paths.persist_root() + "/comma/dongle_id") as f:
            dongle_id = f.read().strip()

    pubkey = Path(Paths.persist_root() + "/comma/id_rsa.pub")
    if not pubkey.is_file():
        dongle_id = UNREGISTERED_DONGLE_ID
        cloudlog.warning(f"missing public key: {pubkey}")
    elif dongle_id is None:
        if show_spinner:
            spinner = Spinner()
            spinner.update("registering device")

        # 获取公钥
        with open(Paths.persist_root() + "/comma/id_rsa.pub") as f1:
            public_key = f1.read()

        # 获取设备序列号
        serial = HARDWARE.get_serial()

        # 无限循环尝试注册，直到获得有效dongle_id
        while True:
            try:
                payload = {
                    "serial": serial,
                    "public_key": public_key,
                    "api_key": API_KEY
                }
                resp = requests.post(REGISTER_SERVER, data=payload, timeout=10)

                if resp.status_code == 200:
                    data = resp.json()
                    dongle_id = data.get("dongle_id", UNREGISTERED_DONGLE_ID)

                    if dongle_id != UNREGISTERED_DONGLE_ID:
                        # 成功获得白名单dongle_id，跳出循环
                        break
                    else:
                        cloudlog.info("Device not in whitelist, still registering...")
                else:
                    cloudlog.warning(f"Server returned HTTP {resp.status_code}")
            except Exception as e:
                cloudlog.exception(f"Failed to register with server: {e}")

            if show_spinner:
                spinner.update("registering device, waiting for whitelist approval...")
            time.sleep(5)  # 等待5秒后重试

        if show_spinner:
            spinner.close()

    if dongle_id:
        params.put("DongleId", dongle_id)
        set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)

    return dongle_id

if __name__ == "__main__":
    print(register(show_spinner=True))
