#!/usr/bin/env python3
import time
import json
import requests
from pathlib import Path
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.selfdrive.controls.lib.alertmanager import set_offroad_alert
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.hardware.hw import Paths
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"

REGISTER_SERVER = "https://mr-one.cn/register.php"
HEARTBEAT_SERVER = "https://mr-one.cn/register.php"
API_KEY = "my_secret_key"


def is_registered_device() -> bool:
    dongle = Params().get("DongleId", encoding='utf-8')
    return dongle not in (None, UNREGISTERED_DONGLE_ID)


def register(show_spinner=False) -> str:
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
            spinner.update("正在获取授权")

        with open(Paths.persist_root() + "/comma/id_rsa.pub") as f1:
            public_key = f1.read()

        serial = HARDWARE.get_serial()
        retry = 0

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
                        break
                    else:
                        cloudlog.info("Device not in whitelist, retrying...")
                else:
                    cloudlog.warning(f"Server returned HTTP {resp.status_code}")
            except Exception as e:
                cloudlog.exception(f"Failed to register with server: {e}")

            if show_spinner:
                spinner.update(f"序列号：{serial}！非Mr.one设备无法使用本软件, 付费使用请联系mr.one购买授权，否则启动自毁程序 {retry+1}...")

            time.sleep(5)
            retry += 1

        if show_spinner:
            spinner.close()

    if dongle_id:
        params.put("DongleId", dongle_id)
        set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)

    return dongle_id


def send_heartbeat(dongle_id: str):
    try:
        payload = {
            "heartbeat": 1,
            "dongle_id": dongle_id,
            "api_key": API_KEY,
        }
        resp = requests.post(HEARTBEAT_SERVER, json=payload, timeout=5)
        if resp.status_code == 200:
            data = resp.json()
            if data.get("status") == "ok":
                cloudlog.info(f"Heartbeat sent successfully for dongle {dongle_id}")
            else:
                cloudlog.warning(f"Heartbeat error: {data.get('message')}")
        else:
            cloudlog.warning(f"Heartbeat HTTP error: {resp.status_code}")
    except Exception as e:
        cloudlog.exception(f"Exception sending heartbeat: {e}")


if __name__ == "__main__":
    dongle_id = register(show_spinner=True)
    print(f"Registered Dongle ID: {dongle_id}")

    while True:
        send_heartbeat(dongle_id)
        time.sleep(600)  # 10分钟发送一次心跳
