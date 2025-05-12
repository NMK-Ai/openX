#!/usr/bin/env python3
import subprocess
import time
from cereal import car, messaging
from openpilot.common.realtime import Ratekeeper
import threading

AudibleAlert = car.CarControl.HUDControl.AudibleAlert

class Beepd:
  def __init__(self):
    self.current_alert = AudibleAlert.none

  def enable_gpio(self):
    subprocess.run("echo 42 | sudo tee /sys/class/gpio/export",
                  shell=True,
                  stderr=subprocess.STDOUT,
                  encoding='utf8')
    subprocess.run("echo \"out\" | sudo tee /sys/class/gpio/gpio42/direction",
                  shell=True,
                  stderr=subprocess.STDOUT,
                  encoding='utf8')

  def _beep(self, on):
    subprocess.run("echo \"{"1" if on else "0"}\" | sudo tee /sys/class/gpio/gpio42/value",
                  shell=True,
                  stderr=subprocess.STDOUT,
                  encoding='utf8')

  def engage(self):
    self.enable_gpio()
    self._beep(True)
    time.sleep(0.05)
    self._beep(False)

  def disengage(self):
    self.enable_gpio()
    for _ in range(2):
      self._beep(True)
      time.sleep(0.01)
      self._beep(False)

  def warning(self):
    self.enable_gpio()
    for _ in range(3):
      self._beep(True)
      time.sleep(0.01)
      self._beep(False)

  def dispatch_beep(self, func):
    threading.Thread(target=func, daemon=True).start()

  def update_alert(self, new_alert):
    current_alert_played_once = self.current_alert == AudibleAlert.none
    if self.current_alert != new_alert and (new_alert != AudibleAlert.none or current_alert_played_once):
      self.current_alert = new_alert
      print(new_alert)
      if self.current_alert == AudibleAlert.engage:
        self.dispatch_beep(self.engage)
      elif self.current_alert == AudibleAlert.disengage:
        self.dispatch_beep(self.disengage)
      elif self.current_alert == AudibleAlert.refuse:
        self.dispatch_beep(self.warning)
      elif self.current_alert == AudibleAlert.prompt:
        self.dispatch_beep(self.warning)
      elif self.current_alert == AudibleAlert.warningSoft:
        self.dispatch_beep(self.warning)

  def get_audible_alert(self, sm):
    if sm.updated['selfdriveState']:
      new_alert = sm['selfdriveState'].alertSound.raw
      self.update_alert(new_alert)

  def test_beepd_thread(self):
    frame = 0
    rk = Ratekeeper(20)
    pm = messaging.PubMaster(['selfdriveState'])
    while True:
      cs = messaging.new_message('selfdriveState')
      if frame == 20:
        cs.selfdriveState.alertSound = AudibleAlert.engage
      if frame == 40:
        cs.selfdriveState.alertSound = AudibleAlert.disengage
      if frame == 60:
        cs.selfdriveState.alertSound = AudibleAlert.prompt

      if frame == 80:
        cs.selfdriveState.alertSound = AudibleAlert.disengage

      if frame == 85:
        cs.selfdriveState.alertSound = AudibleAlert.prompt

      pm.send("selfdriveState", cs)
      print("aa")
      frame += 1
      rk.keep_time()

  def beepd_thread(self):
    sm = messaging.SubMaster(['selfdriveState'])
    rk = Ratekeeper(20)
    # threading.Thread(target=self.test_beepd_thread, daemon=True).start()

    while True:
      sm.update(0)
      self.get_audible_alert(sm)
      rk.keep_time()

def main():
  s = Beepd()
  s.beepd_thread()


if __name__ == "__main__":
  main()
