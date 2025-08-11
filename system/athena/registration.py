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

        # 获取公钥私钥（目前保留，未来可用于签名）
        with open(Paths.persist_root() + "/comma/id_rsa.pub") as f1:
            public_key = f1.read()

        # 获取设备序列号
        serial = HARDWARE.get_serial()

        # 10秒内获取IMEI，否则直接返回
        start_time = time.monotonic()
        imei1 = imei2 = None
        while imei1 is None and imei2 is None:
            try:
                imei1, imei2 = HARDWARE.get_imei(0), HARDWARE.get_imei(1)
            except Exception:
                cloudlog.exception("Error getting imei, retrying...")
                time.sleep(1)

            if time.monotonic() - start_time > 10:  # 超过10秒直接放弃
                if show_spinner:
                    spinner.close()
                cloudlog.warning("IMEI fetch timeout, returning unregistered ID")
                return UNREGISTERED_DONGLE_ID

        # 调用你自己的注册服务器
        try:
            payload = {
                "serial": serial,
                "imei1": imei1,
                "imei2": imei2,
                "public_key": public_key,
                "api_key": API_KEY
            }
            resp = requests.post(REGISTER_SERVER, data=payload, timeout=10)

            if resp.status_code == 200:
                data = resp.json()
                if "dongle_id" in data:
                    dongle_id = data["dongle_id"]
                else:
                    dongle_id = UNREGISTERED_DONGLE_ID
            else:
                cloudlog.warning(f"Server returned HTTP {resp.status_code}")
                dongle_id = UNREGISTERED_DONGLE_ID
        except Exception as e:
            cloudlog.exception(f"Failed to register with server: {e}")
            dongle_id = UNREGISTERED_DONGLE_ID

        if show_spinner:
            spinner.close()

    if dongle_id:
        params.put("DongleId", dongle_id)
        set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)

    return dongle_id

if __name__ == "__main__":
    print(register(show_spinner=True))
