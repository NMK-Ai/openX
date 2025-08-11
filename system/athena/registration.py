import time
import requests
from pathlib import Path
from openpilot.common.params import Params
from openpilot.common.swaglog import cloudlog
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.hardware.hw import Paths
from openpilot.selfdrive.selfdrived.alertmanager import set_offroad_alert

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"
REGISTER_SERVER = "https://mr-one.cn/register.php"
API_KEY = "my_secret_key"

def register(show_spinner=False) -> str:
    params = Params()

    dongle_id = params.get("DongleId", encoding='utf8')
    if dongle_id is None and Path(Paths.persist_root() + "/comma/dongle_id").is_file():
        with open(Paths.persist_root() + "/comma/dongle_id") as f:
            dongle_id = f.read().strip()

    pubkey = Path(Paths.persist_root() + "/comma/id_rsa.pub")
    if not pubkey.is_file():
        cloudlog.warning(f"missing public key: {pubkey}")
        dongle_id = UNREGISTERED_DONGLE_ID
        params.put("DongleId", dongle_id)
        set_offroad_alert("Offroad_UnofficialHardware", True)
        return dongle_id

    if dongle_id is not None:
        # 已注册过，直接返回
        return dongle_id

    # 读取公钥
    with open(pubkey) as f:
        public_key = f.read()

    serial = HARDWARE.get_serial()

    if show_spinner:
        from openpilot.common.spinner import Spinner
        spinner = Spinner()
        spinner.update("Waiting for device to be registered...")

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
                    cloudlog.info(f"Device registered successfully with dongle_id: {dongle_id}")
                    break
                else:
                    cloudlog.info("Device not in whitelist, still waiting for registration...")
            else:
                cloudlog.warning(f"Registration server returned HTTP {resp.status_code}")

        except Exception as e:
            cloudlog.warning(f"Registration attempt failed: {e}")

        if show_spinner:
            spinner.update("Waiting for device to be registered...")

        time.sleep(5)

    if show_spinner:
        spinner.close()

    params.put("DongleId", dongle_id)
    set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)

    return dongle_id
