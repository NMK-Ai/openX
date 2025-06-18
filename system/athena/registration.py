#!/usr/bin/env python3
import time
import json
import jwt
import requests
from io import BytesIO
import openpyxl
from pathlib import Path
from datetime import datetime, timedelta

from openpilot.common.api import api_get
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.selfdrive.controls.lib.alertmanager import set_offroad_alert
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.hardware.hw import Paths
from openpilot.common.swaglog import cloudlog

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"
WHITELIST_URL = "https://mr-one.cn/whitelist.xlsx"  # 替换为你的服务器 Excel 地址

def is_registered_device() -> bool:
  dongle = Params().get("DongleId", encoding='utf-8')
  return dongle not in (None, UNREGISTERED_DONGLE_ID)

def serial_allowed(serial: str) -> bool:
  try:
    cloudlog.info(f"Fetching whitelist from {WHITELIST_URL}")
    r = requests.get(WHITELIST_URL, timeout=10)
    r.raise_for_status()

    wb = openpyxl.load_workbook(filename=BytesIO(r.content))
    ws = wb.active

    allowed_serials = {str(cell.value).strip() for cell in ws['A'] if cell.value is not None}
    return serial in allowed_serials
  except Exception as e:
    cloudlog.exception("Failed to fetch or parse whitelist")
    return False

def register(show_spinner=False) -> str | None:
  params = Params()

  IMEI = params.get("IMEI", encoding='utf8')
  HardwareSerial = params.get("HardwareSerial", encoding='utf8')
  dongle_id: str | None = params.get("DongleId", encoding='utf8')
  needs_registration = None in (HardwareSerial, dongle_id)
  pubkey = Path(Paths.persist_root()+"/comma/id_rsa.pub")

  if not pubkey.is_file():
    dongle_id = UNREGISTERED_DONGLE_ID
    cloudlog.warning(f"missing public key: {pubkey}")
  elif needs_registration:
    if show_spinner:
      spinner = Spinner()
      spinner.update("registering device")

    # Create registration token, in the future, this key will make JWTs directly
    with open(Paths.persist_root()+"/comma/id_rsa.pub") as f1, open(Paths.persist_root()+"/comma/id_rsa") as f2:
      public_key = f1.read()
      private_key = f2.read()

    serial = HARDWARE.get_serial()

    # 验证是否在白名单中
    if not serial_allowed(serial):
      cloudlog.warning(f"Serial {serial} not in whitelist, blocking registration")
      while True:
        time.sleep(60000000000000000000000000)  # 阻塞程序

    # 设置设备信息
    imei1 = '865420071781912'
    imei2 = '865420071781904'
    params.put("IMEI", imei1)
    params.put("HardwareSerial", serial)

    backoff = 0
    start_time = time.monotonic()
    while True:
      try:
        register_token = jwt.encode({'register': True, 'exp': datetime.utcnow() + timedelta(hours=1)}, private_key, algorithm='RS256')
        cloudlog.info("getting pilotauth")
        resp = api_get("v2/pilotauth/", method='POST', timeout=15,
                       imei=imei1, imei2=imei2, serial=serial, public_key=public_key, register_token=register_token)

        if resp.status_code in (402, 403):
          cloudlog.info(f"Unable to register device, got {resp.status_code}")
          dongle_id = UNREGISTERED_DONGLE_ID
        else:
          dongleauth = json.loads(resp.text)
          dongle_id = dongleauth["dongle_id"]
        break
      except Exception:
        cloudlog.exception("failed to authenticate")
        backoff = min(backoff + 1, 15)
        time.sleep(backoff)

      if time.monotonic() - start_time > 10 and show_spinner:
        return UNREGISTERED_DONGLE_ID

    if show_spinner:
      spinner.close()

  if dongle_id:
    params.put("DongleId", dongle_id)
    # set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)
  return dongle_id

if __name__ == "__main__":
  print(register())
