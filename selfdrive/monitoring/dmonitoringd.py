#!/usr/bin/env python3
import gc

import cereal.messaging as messaging
from openpilot.common.params import Params
from openpilot.common.realtime import set_realtime_priority
from openpilot.selfdrive.monitoring.helpers import DriverMonitoring


def dmonitoringd_thread():
  gc.disable()
  set_realtime_priority(2)

  params = Params()
  # 强制写入为左舵模式（防止历史值或其他模块干扰）
  params.put_bool_nonblocking("IsRhdDetected", False)

  pm = messaging.PubMaster(['driverMonitoringState', 'driverMonitoringStateSP'])
  sm = messaging.SubMaster(['driverStateV2', 'liveCalibration', 'carState', 'controlsState', 'modelV2'], poll='driverStateV2')

  # 强制左舵模式运行 DM
  DM = DriverMonitoring(
    rhd_saved=False,
    always_on=params.get_bool("AlwaysOnDM"),
    hands_on_wheel_monitoring=params.get_bool("HandsOnWheelMonitoring")
  )

  # 20Hz <- dmonitoringmodeld
  while True:
    sm.update()
    if not sm.updated['driverStateV2']:
      continue

    valid = sm.all_checks()
    if valid:
      DM.run_step(sm)

    # publish
    dat = DM.get_state_packet(valid=valid)
    pm.send('driverMonitoringState', dat)

    sp_dat = DM.get_sp_state_packet(valid=valid)
    pm.send('driverMonitoringStateSP', sp_dat)

    # load live always-on toggle
    if sm['driverStateV2'].frameId % 40 == 1:
      DM.always_on = params.get_bool("AlwaysOnDM")
      DM.hands_on_wheel_monitoring = params.get_bool("HandsOnWheelMonitoring")

    # 强制持续写回左舵模式，防止被覆盖（每5分钟刷新一次）
    if sm['driverStateV2'].frameId % 6000 == 0:
      params.put_bool_nonblocking("IsRhdDetected", False)

def main():
  dmonitoringd_thread()


if __name__ == '__main__':
  main()
