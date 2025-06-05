from cereal import car
from openpilot.common.realtime import DT_CTRL
from opendbc.can.packer import CANPacker
from openpilot.selfdrive.car import apply_std_steer_angle_limits
from openpilot.selfdrive.car.interfaces import CarControllerBase
from openpilot.selfdrive.car.nissan import nissancan
from openpilot.selfdrive.car.nissan.values import CAR, CarControllerParams

VisualAlert = car.CarControl.HUDControl.VisualAlert


class SteerRateLimiter:
  def __init__(self):
    self.last_angle = 0.0
    self.last_delta = 0.0
    self.filtered_target = 0.0  # 新增：用于低通滤波

  def smooth(self, desired_angle, v_ego):
    """
    限制角速度和角加速度，并加入一阶低通滤波以防止低速方向盘抖动。
    """
    # 起步/停车状态完全锁死方向角
    if v_ego < 0.5:
      return self.last_angle

    # 一阶低通滤波器
    alpha = 0.2 if v_ego < 10 else 0.5
    self.filtered_target = (1 - alpha) * self.filtered_target + alpha * desired_angle
    desired_angle = self.filtered_target

    # 各速度段对应最大角速度（deg/frame）
    if v_ego < 3:
      max_delta = 0.05
    elif v_ego < 10:
      max_delta = 0.2
    elif v_ego < 25:
      max_delta = 0.8
    else:
      max_delta = 2.5

    # 限制角速度
    raw_delta = desired_angle - self.last_angle
    delta = max(-max_delta, min(max_delta, raw_delta))

    # 限制角加速度（避免角度跳变）
    max_delta_delta = 0.3  # deg/frame²
    delta_delta = delta - self.last_delta
    delta_delta = max(-max_delta_delta, min(max_delta_delta, delta_delta))

    # 更新状态
    delta = self.last_delta + delta_delta
    self.last_delta = delta
    self.last_angle += delta

    return self.last_angle


class CarController(CarControllerBase):
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.car_fingerprint = CP.carFingerprint
    self.frame = 0

    self.lkas_max_torque = 0
    self.apply_angle_last = 0

    self.packer = CANPacker(dbc_name)

    self.disengage_blink = 0.
    self.lat_disengage_init = False
    self.lat_active_last = False

    self.steer_limiter = SteerRateLimiter()

  def update(self, CC, CS, now_nanos):
    actuators = CC.actuators
    hud_control = CC.hudControl
    pcm_cancel_cmd = CC.cruiseControl.cancel

    lateral_paused = CS.madsEnabled and not CC.latActive
    if CC.latActive:
      self.lat_disengage_init = False
    elif self.lat_active_last:
      self.lat_disengage_init = True

    if not self.lat_disengage_init:
      self.disengage_blink = self.frame

    blinking_icon = (self.frame - self.disengage_blink) * DT_CTRL < 1.0 if self.lat_disengage_init else False

    self.lat_active_last = CC.latActive

    can_sends = []

    ### STEER ###
    steer_hud_alert = 1 if hud_control.visualAlert in (VisualAlert.steerRequired, VisualAlert.ldw) else 0

    if CC.latActive:
      raw_angle = apply_std_steer_angle_limits(actuators.steeringAngleDeg, self.apply_angle_last, CS.out.vEgoRaw, CarControllerParams)
      apply_angle = self.steer_limiter.smooth(raw_angle, CS.out.vEgoRaw)

      if not bool(CS.out.steeringPressed):
        self.lkas_max_torque = CarControllerParams.LKAS_MAX_TORQUE
      else:
        self.lkas_max_torque = max(
          CarControllerParams.LKAS_MAX_TORQUE * 0.5,
          CarControllerParams.LKAS_MAX_TORQUE - 0.6 * max(0, abs(CS.out.steeringTorque) - CarControllerParams.STEER_THRESHOLD)
        )
    else:
      apply_angle = CS.out.steeringAngleDeg
      self.lkas_max_torque = 0

    self.apply_angle_last = apply_angle

    # ACC Cancel 命令
    if self.CP.carFingerprint in (CAR.NISSAN_ROGUE, CAR.NISSAN_XTRAIL, CAR.NISSAN_ALTIMA) and pcm_cancel_cmd:
      can_sends.append(nissancan.create_acc_cancel_cmd(self.packer, self.car_fingerprint, CS.cruise_throttle_msg))

    if self.CP.carFingerprint in (CAR.NISSAN_LEAF, CAR.NISSAN_LEAF_IC) and self.frame % 2 == 0:
      can_sends.append(nissancan.create_cancel_msg(self.packer, CS.cancel_msg, pcm_cancel_cmd))

    # 方向盘控制
    can_sends.append(nissancan.create_steering_control(
      self.packer, apply_angle, self.frame, CC.latActive, self.lkas_max_torque))

    # HUD 显示
    if self.CP.carFingerprint != CAR.NISSAN_ALTIMA:
      if self.frame % 2 == 0:
        can_sends.append(nissancan.create_lkas_hud_msg(self.packer, CS.lkas_hud_msg, CC.latActive, blinking_icon, lateral_paused,
                                                       hud_control.leftLaneVisible, hud_control.rightLaneVisible,
                                                       hud_control.leftLaneDepart, hud_control.rightLaneDepart))

      if self.frame % 50 == 0:
        can_sends.append(nissancan.create_lkas_hud_info_msg(
          self.packer, CS.lkas_hud_info_msg, steer_hud_alert
        ))

    # 输出角度记录
    new_actuators = actuators.as_builder()
    new_actuators.steeringAngleDeg = apply_angle

    self.frame += 1
    return new_actuators, can_sends
