#!/usr/bin/env python3
import time
import json
import jwt
from pathlib import Path

from datetime import datetime, timedelta
from openpilot.common.api import api_get
from openpilot.common.params import Params
from openpilot.common.spinner import Spinner
from openpilot.selfdrive.controls.lib.alertmanager import set_offroad_alert
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.hardware.hw import Paths
from openpilot.common.swaglog import cloudlog

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
                msg = dongleauth.get("message", "Unknown error")
                #cloudlog.warning(f"Registration failed: {msg}")
                if spinner:
                    spinner.update(f"Registration failed for serial={serial}, retrying...")

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

UNREGISTERED_DONGLE_ID = "UnregisteredDevice"


def is_registered_device() -> bool:
  dongle = Params().get("DongleId", encoding='utf-8')
  return dongle not in (None, UNREGISTERED_DONGLE_ID)


def register(show_spinner=False) -> str | None:
  params = Params()
  return UNREGISTERED_DONGLE_ID
  IMEI = params.get("IMEI", encoding='utf8')
  HardwareSerial = params.get("HardwareSerial", encoding='utf8')
  dongle_id: str | None = params.get("DongleId", encoding='utf8')
  needs_registration = None in (IMEI, HardwareSerial, dongle_id)
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

    # Block until we get the imei
    serial = HARDWARE.get_serial()
    start_time = time.monotonic()
    imei1='865420071781912'
    imei2='865420071781904'
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
    #set_offroad_alert("Offroad_UnofficialHardware", (dongle_id == UNREGISTERED_DONGLE_ID) and not PC)
  return dongle_id


if __name__ == "__main__":
  print(register())
