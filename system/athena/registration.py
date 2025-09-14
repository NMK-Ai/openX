#!/usr/bin/env python3
import time
import json
import requests
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.system.hardware import HARDWARE
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"
REGISTER_URL = "https://mr-one.cn/v2/register.php"  # Replace with your registration server

def is_registered_device() -> bool:
    dongle = Params().get("DongleId")
    return dongle not in (None, UNREGISTERED_DONGLE_ID)

def register(show_spinner=True) -> str:
    """
    Register the device. 
    If registration fails, it will loop indefinitely and display error messages.
    Successful registration immediately writes to Params to allow openpilot startup.
    """
    params = Params()
    spinner = Spinner() if show_spinner else None

    while True:
        serial = HARDWARE.get_serial()
        if spinner:
            spinner.update(f"Registering device... serial={serial}")

        try:
            resp = requests.post(
                REGISTER_URL,
                json={"serial": serial},
                timeout=5,
                verify=False  # Disable SSL verification if self-signed
            )

            #cloudlog.info(f"HTTP status code: {resp.status_code}")

            try:
                dongleauth = resp.json()
            except json.JSONDecodeError:
                dongleauth = {}
                #cloudlog.warning("Registration server returned invalid JSON")

            #cloudlog.info(f"Server response: {dongleauth}")

            if dongleauth.get("status") == "ok" and dongleauth.get("dongle_id"):
                dongle_id = dongleauth["dongle_id"]
                params.put("DongleId", dongle_id)
                #cloudlog.info(f"Registration successful, dongle_id={dongle_id}")
                if spinner:
                    spinner.close()
                return dongle_id
            else:
                msg = dongleauth.get("message", "Unknown error")
                #cloudlog.warning(f"Registration failed: {msg}")
                if spinner:
                    spinner.update(f"Registration failed for serial={serial}, retrying...")

        except requests.exceptions.RequestException as e:
            #cloudlog.exception("Registration request failed")
            if spinner:
                spinner.update(f"Network error: {e}, retrying...")

        # Retry every 5 seconds
        time.sleep(5)

if __name__ == "__main__":
    dongle_id = register(show_spinner=True)
    print(f"Registration complete, dongle_id={dongle_id}")
