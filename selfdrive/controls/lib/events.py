#!/usr/bin/env python3
import bisect
import math
import os
from enum import IntEnum
from collections.abc import Callable
from types import SimpleNamespace

from cereal import log, car
import cereal.messaging as messaging
from openpilot.common.conversions import Conversions as CV
from openpilot.common.git import get_short_branch
from openpilot.common.params import Params
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.locationd.calibrationd import MIN_SPEED_FILTER

AlertSize = log.ControlsState.AlertSize
AlertStatus = log.ControlsState.AlertStatus
VisualAlert = car.CarControl.HUDControl.VisualAlert
AudibleAlert = car.CarControl.HUDControl.AudibleAlert
EventName = car.CarEvent.EventName


# Alert priorities
class Priority(IntEnum):
  LOWEST = 0
  LOWER = 1
  LOW = 2
  MID = 3
  HIGH = 4
  HIGHEST = 5


# Event types
class ET:
  ENABLE = 'enable'
  PRE_ENABLE = 'preEnable'
  OVERRIDE_LATERAL = 'overrideLateral'
  OVERRIDE_LONGITUDINAL = 'overrideLongitudinal'
  NO_ENTRY = 'noEntry'
  WARNING = 'warning'
  USER_DISABLE = 'userDisable'
  SOFT_DISABLE = 'softDisable'
  IMMEDIATE_DISABLE = 'immediateDisable'
  PERMANENT = 'permanent'


# get event name from enum
EVENT_NAME = {v: k for k, v in EventName.schema.enumerants.items()}


class Events:
  def __init__(self):
    self.events: list[int] = []
    self.static_events: list[int] = []
    self.event_counters = dict.fromkeys(EVENTS.keys(), 0)

  @property
  def names(self) -> list[int]:
    return self.events

  def __len__(self) -> int:
    return len(self.events)

  def add(self, event_name: int, static: bool=False) -> None:
    if static:
      bisect.insort(self.static_events, event_name)
    bisect.insort(self.events, event_name)

  def clear(self) -> None:
    self.event_counters = {k: (v + 1 if k in self.events else 0) for k, v in self.event_counters.items()}
    self.events = self.static_events.copy()

  def contains(self, event_type: str) -> bool:
    return any(event_type in EVENTS.get(e, {}) for e in self.events)

  def create_alerts(self, event_types: list[str], callback_args=None):
    if callback_args is None:
      callback_args = []

    ret = []
    for e in self.events:
      types = EVENTS[e].keys()
      for et in event_types:
        if et in types:
          alert = EVENTS[e][et]
          if not isinstance(alert, Alert):
            alert = alert(*callback_args)

          if DT_CTRL * (self.event_counters[e] + 1) >= alert.creation_delay:
            alert.alert_type = f"{EVENT_NAME[e]}/{et}"
            alert.event_type = et
            ret.append(alert)
    return ret

  def add_from_msg(self, events):
    for e in events:
      bisect.insort(self.events, e.name.raw)

  def to_msg(self):
    ret = []
    for event_name in self.events:
      event = car.CarEvent.new_message()
      event.name = event_name
      for event_type in EVENTS.get(event_name, {}):
        setattr(event, event_type, True)
      ret.append(event)
    return ret


class Alert:
  def __init__(self,
               alert_text_1: str,
               alert_text_2: str,
               alert_status: log.ControlsState.AlertStatus,
               alert_size: log.ControlsState.AlertSize,
               priority: Priority,
               visual_alert: car.CarControl.HUDControl.VisualAlert,
               audible_alert: car.CarControl.HUDControl.AudibleAlert,
               duration: float,
               alert_rate: float = 0.,
               creation_delay: float = 0.):

    self.alert_text_1 = alert_text_1
    self.alert_text_2 = alert_text_2
    self.alert_status = alert_status
    self.alert_size = alert_size
    self.priority = priority
    self.visual_alert = visual_alert
    self.audible_alert = audible_alert

    self.duration = int(duration / DT_CTRL)

    self.alert_rate = alert_rate
    self.creation_delay = creation_delay

    self.alert_type = ""
    self.event_type: str | None = None

  def __str__(self) -> str:
    return f"{self.alert_text_1}/{self.alert_text_2} {self.priority} {self.visual_alert} {self.audible_alert}"

  def __gt__(self, alert2) -> bool:
    if not isinstance(alert2, Alert):
      return False
    return self.priority > alert2.priority


class NoEntryAlert(Alert):
  def __init__(self, alert_text_2: str,
               alert_text_1: str = "القائد الآلي غير متاح",
               visual_alert: car.CarControl.HUDControl.VisualAlert=VisualAlert.none):
    super().__init__(alert_text_1, alert_text_2, AlertStatus.normal,
                     AlertSize.mid, Priority.LOW, visual_alert,
                     AudibleAlert.refuse, 3.)


class SoftDisableAlert(Alert):
  def __init__(self, alert_text_2: str):
    super().__init__("تولَّ القيادة فورًا", alert_text_2,
                     AlertStatus.userPrompt, AlertSize.full,
                     Priority.MID, VisualAlert.steerRequired,
                     AudibleAlert.warningSoft, 2.),


# less harsh version of SoftDisable, where the condition is user-triggered
class UserSoftDisableAlert(SoftDisableAlert):
  def __init__(self, alert_text_2: str):
    super().__init__(alert_text_2),
    self.alert_text_1 = "سيتم فصل القائد الآلي"


class ImmediateDisableAlert(Alert):
  def __init__(self, alert_text_2: str):
    super().__init__("تولَّ القيادة فورًا", alert_text_2,
                     AlertStatus.critical, AlertSize.full,
                     Priority.HIGHEST, VisualAlert.steerRequired,
                     AudibleAlert.warningImmediate, 4.),


class EngagementAlert(Alert):
  def __init__(self, audible_alert: car.CarControl.HUDControl.AudibleAlert):
    super().__init__("", "",
                     AlertStatus.normal, AlertSize.none,
                     Priority.MID, VisualAlert.none,
                     audible_alert, .2),


class NormalPermanentAlert(Alert):
  def __init__(self, alert_text_1: str, alert_text_2: str = "", duration: float = 0.2, priority: Priority = Priority.LOWER, creation_delay: float = 0.):
    super().__init__(alert_text_1, alert_text_2,
                     AlertStatus.normal, AlertSize.mid if len(alert_text_2) else AlertSize.small,
                     priority, VisualAlert.none, AudibleAlert.none, duration, creation_delay=creation_delay),


class StartupAlert(Alert):
  def __init__(self, alert_text_1: str, alert_text_2: str = "احرص دائمًا أن تبقى يداك على المقود وعيناك على الطريق", alert_status=AlertStatus.normal):
    super().__init__(alert_text_1, alert_text_2,
                     alert_status, AlertSize.mid,
                     Priority.LOWER, VisualAlert.none, AudibleAlert.none, 5.),


# ********** helper functions **********
def get_display_speed(speed_ms: float, metric: bool) -> str:
  speed = int(round(speed_ms * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH)))
  unit = 'km/h' if metric else 'mph'
  return f"{speed} {unit}"


# ********** alert callback functions **********

AlertCallbackType = Callable[[car.CarParams, car.CarState, messaging.SubMaster, bool, int], Alert]


def soft_disable_alert(alert_text_2: str) -> AlertCallbackType:
  def func(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
    if soft_disable_time < int(0.5 / DT_CTRL):
      return ImmediateDisableAlert(alert_text_2)
    return SoftDisableAlert(alert_text_2)
  return func

def user_soft_disable_alert(alert_text_2: str) -> AlertCallbackType:
  def func(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
    if soft_disable_time < int(0.5 / DT_CTRL):
      return ImmediateDisableAlert(alert_text_2)
    return UserSoftDisableAlert(alert_text_2)
  return func

def startup_master_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  branch = get_short_branch()  # Ensure get_short_branch is cached to avoid lags on startup
  if "REPLAY" in os.environ:
    branch = "replay"

  return StartupAlert("تحذير: هذا الفرع غير مُختبَر", branch, alert_status=AlertStatus.userPrompt)

def below_engage_speed_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  return NoEntryAlert(f"قدْ بسرعة أعلى من {get_display_speed(CP.minEnableSpeed, metric)} للتشغيل")


def below_steer_speed_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  return Alert(
    f"التوجيه غير متاح تحت {get_display_speed(CP.minSteerSpeed, metric)}",
    "",
    AlertStatus.userPrompt, AlertSize.small,
    Priority.LOW, VisualAlert.steerRequired, AudibleAlert.prompt, 0.4)


def calibration_incomplete_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  first_word = 'إعادة المعايرة' if sm['liveCalibration'].calStatus == log.LiveCalibrationData.Status.recalibrating else 'المعايرة'
  return Alert(
    f"{first_word} جارية: {sm['liveCalibration'].calPerc:.0f}%",
    f"قدْ بسرعة أعلى من {get_display_speed(MIN_SPEED_FILTER, metric)}",
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .2)


# *** debug alerts ***

def out_of_space_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  full_perc = round(100. - sm['deviceState'].freeSpacePercent)
  return NormalPermanentAlert("نفادت المساحة التخزينية", f"{full_perc}% ممتلئة")


def posenet_invalid_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  mdl = sm['modelV2'].velocity.x[0] if len(sm['modelV2'].velocity.x) else math.nan
  err = CS.vEgo - mdl
  msg = f"خطأ السرعة: {err:.1f} m/s"
  return NoEntryAlert(msg, alert_text_1="سرعة Posenet غير صالحة")


def process_not_running_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  not_running = [p.name for p in sm['managerState'].processes if not p.running and p.shouldBeRunning]
  msg = ', '.join(not_running)
  return NoEntryAlert(msg, alert_text_1="العملية غير قيد التشغيل")


def comm_issue_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  bs = [s for s in sm.data.keys() if not sm.all_checks([s, ])]
  msg = ', '.join(bs[:4])  # can't fit too many on one line
  return NoEntryAlert(msg, alert_text_1="مشكلة اتصال بين العمليات")


def camera_malfunction_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  all_cams = ('roadCameraState', 'driverCameraState', 'wideRoadCameraState')
  bad_cams = [s.replace('State', '') for s in all_cams if s in sm.data.keys() and not sm.all_checks([s, ])]
  return NormalPermanentAlert("خلل في الكاميرا", ', '.join(bad_cams))


def calibration_invalid_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  rpy = sm['liveCalibration'].rpyCalib
  yaw = math.degrees(rpy[2] if len(rpy) == 3 else math.nan)
  pitch = math.degrees(rpy[1] if len(rpy) == 3 else math.nan)
  angles = f"أعد تركيب الجهاز (الميل: {pitch:.1f}°، الانحراف: {yaw:.1f}°)"
  return NormalPermanentAlert("المعايرة غير صالحة", angles)


def overheat_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  cpu = max(sm['deviceState'].cpuTempC, default=0.)
  gpu = max(sm['deviceState'].gpuTempC, default=0.)
  temp = max((cpu, gpu, sm['deviceState'].memoryTempC))
  return NormalPermanentAlert("ارتفاع حرارة النظام", f"{temp:.0f} °C")


def low_memory_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  return NormalPermanentAlert("ذاكرة منخفضة", f"{sm['deviceState'].memoryUsagePercent}% مستخدمة")


def high_cpu_usage_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  x = max(sm['deviceState'].cpuUsagePercent, default=0.)
  return NormalPermanentAlert("ارتفاع استخدام المعالج", f"{x}% مستخدم")


def modeld_lagging_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  return NormalPermanentAlert("نموذج القيادة متأخر", f"{sm['modelV2'].frameDropPerc:.1f}% إطارات مفقودة")


def wrong_car_mode_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  if frogpilot_toggles.has_cc_long:
    text = "فعّل مثبت السرعة للتشغيل"
  elif CP.carName == "honda":
    text = "فعّل المفتاح الرئيسي للتشغيل"
  else:
    text = "فعّل مثبت السرعة التكيفي للتشغيل"
  return NoEntryAlert(text)


def joystick_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  axes = sm['testJoystick'].axes
  gb, steer = list(axes)[:2] if len(axes) else (0., 0.)
  vals = f"دعسة الوقود: {round(gb * 100.)}%, التوجيه: {round(steer * 100.)}%"
  return NormalPermanentAlert("وضع ذراع التحكم", vals)


# FrogPilot alerts (نصوص ظاهرة فقط)
def custom_startup_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  return StartupAlert(frogpilot_toggles.startup_alert_top, frogpilot_toggles.startup_alert_bottom, alert_status=AlertStatus.frogpilot)


def forcing_stop_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  model_length = sm["frogpilotPlan"].forcingStopLength
  model_length_msg = f"{model_length:.1f} meters" if metric else f"{model_length * CV.METER_TO_FOOT:.1f} feet"

  return Alert(
    f"سيتم إيقاف السيارة خلال {model_length_msg}",
    "اضغط دواسة الوقود أو زر 'Resume' لتجاوز الأمر",
    AlertStatus.frogpilot, AlertSize.mid,
    Priority.MID, VisualAlert.none, AudibleAlert.prompt, 1.)


def holiday_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  holiday_messages = {
    "new_years": "سنة جديدة سعيدة! 🎉",
    "valentines": "عيد حب سعيد! ❤️",
    "st_patricks": "عيد القديس باتريك سعيد! 🍀",
    "world_frog_day": "يوم الضفدع العالمي سعيد! 🐸",
    "april_fools": "كذبة أبريل سعيدة! 🤡",
    "easter_week": "عيد فصح سعيد! 🐰",
    "may_the_fourth": "May the 4th be with you! 🚀",
    "cinco_de_mayo": "¡سينكو دي مايو سعيد! 🌮",
    "stitch_day": "يوم ستيتش سعيد! 💙",
    "fourth_of_july": "عيد الاستقلال سعيد! 🎆",
    "halloween_week": "هالوين سعيد! 🎃",
    "thanksgiving_week": "عيد الشكر سعيد! 🦃",
    "christmas_week": "عيد ميلاد مجيد! 🎄",
  }

  return Alert(
    holiday_messages.get(frogpilot_toggles.current_holiday_theme),
    "",
    AlertStatus.normal, AlertSize.small,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.startup, 5.)


def no_lane_available_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  lane_width = sm["frogpilotPlan"].laneWidthLeft if CS.leftBlinker else sm["frogpilotPlan"].laneWidthRight
  lane_width_msg = f"{lane_width:.1f} meters" if metric else f"{lane_width * CV.METER_TO_FOOT:.1f} feet"

  return Alert(
    "لا يوجد مسار متاح",
    f"عرض المسار المرصود فقط {lane_width_msg}",
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .2)


def torque_nn_load_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int, frogpilot_toggles: SimpleNamespace) -> Alert:
  model_name = Params().get("NNFFModelName", encoding="utf-8")
  if model_name is None:
    return Alert(
      "متحكم العزم NNFF غير متاح",
      "تَبَرَّع بالسجلات لـ Twilsonco لدعم سيارتك!",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, 10.0)
  else:
    return Alert(
      "تم تحميل متحكم العزم NNFF",
      model_name,
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.engage, 5.0)


EVENTS: dict[int, dict[str, Alert | AlertCallbackType]] = {
  # ********** events with no alerts **********

  EventName.stockFcw: {},
  EventName.actuatorsApiUnavailable: {},

  # ********** events only containing alerts displayed in all states **********

  EventName.joystickDebug: {
    ET.WARNING: joystick_alert,
    ET.PERMANENT: NormalPermanentAlert("وضع ذراع التحكم"),
  },

  EventName.controlsInitializing: {
    ET.NO_ENTRY: NoEntryAlert("جاري تهيئة النظام"),
  },

  EventName.startup: {
    ET.PERMANENT: StartupAlert("كن مستعدًا لتولّي القيادة في أي وقت")
  },

  EventName.startupMaster: {
    ET.PERMANENT: startup_master_alert,
  },

  # Car is recognized, but marked as dashcam only
  EventName.startupNoControl: {
    ET.PERMANENT: StartupAlert("وضع الداش كام"),
    ET.NO_ENTRY: NoEntryAlert("وضع الداش كام"),
  },

  # Car is not recognized
  EventName.startupNoCar: {
    ET.PERMANENT: StartupAlert("وضع الداش كام لسيارة غير مدعومة"),
  },

  EventName.startupNoFw: {
    ET.PERMANENT: StartupAlert("السيارة غير معرّفة",
                               "تحقق من توصيلات طاقة NMK",
                               alert_status=AlertStatus.userPrompt),
  },

  EventName.startupNoSecOcKey: {
    ET.PERMANENT: NormalPermanentAlert("وضع الداش كام",
                                       "مفتاح الأمان غير متاح",
                                       priority=Priority.HIGH),
  },

  EventName.dashcamMode: {
    ET.PERMANENT: NormalPermanentAlert("وضع الداش كام",
                                       priority=Priority.LOWEST),
  },

  EventName.invalidLkasSetting: {
    ET.PERMANENT: NormalPermanentAlert("نظام LKAS الأصلي مفعّل",
                                       "أوقف نظام LKAS الأصلي للتشغيل"),
  },

  EventName.cruiseMismatch: {
    #ET.PERMANENT: ImmediateDisableAlert("القائد الآلي فشل في إلغاء المثبت"),
  },

  # القائد الآلي لا يتعرف على السيارة (وضع القراءة فقط)
  EventName.carUnrecognized: {
    ET.PERMANENT: NormalPermanentAlert("وضع الداش كام",
                                       "السيارة غير معرّفة",
                                       priority=Priority.LOWEST),
  },

  EventName.stockAeb: {
    ET.PERMANENT: Alert(
      "فرامل!",
      "نظام AEB الأصلي: خطر اصطدام",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 2.),
    ET.NO_ENTRY: NoEntryAlert("نظام AEB الأصلي: خطر اصطدام"),
  },

  EventName.fcw: {
    ET.PERMANENT: Alert(
      "فرامل!",
      "خطر اصطدام",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.warningSoft, 2.),
  },

  EventName.ldw: {
    ET.PERMANENT: Alert(
      "تم رصد خروج عن المسار",
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.ldw, AudibleAlert.prompt, 3.),
  },

  # ********** events only containing alerts that display while engaged **********

  EventName.steerTempUnavailableSilent: {
    ET.WARNING: Alert(
      "التوجيه غير متاح مؤقتًا",
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.prompt, 1.8),
  },

  EventName.preDriverDistracted: {
    ET.PERMANENT: Alert(
      "انتبه",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.promptDriverDistracted: {
    ET.PERMANENT: Alert(
      "انتبه",
      "السائق مُشتَّت",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.promptDistracted, .1),
  },

  EventName.driverDistracted: {
    ET.PERMANENT: Alert(
      "افصل فورًا",
      "السائق مُشتَّت",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.warningImmediate, .1),
  },

  EventName.preDriverUnresponsive: {
    ET.PERMANENT: Alert(
      "المس عجلة القيادة: لا يوجد وجه",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.promptDriverUnresponsive: {
    ET.PERMANENT: Alert(
      "المس عجلة القيادة",
      "السائق لا يستجيب",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.promptDistracted, .1),
  },

  EventName.driverUnresponsive: {
    ET.PERMANENT: Alert(
      "افصل فورًا",
      "السائق لا يستجيب",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.warningImmediate, .1),
  },

  EventName.manualRestart: {
    ET.WARNING: Alert(
      "تولَّ القيادة",
      "أكمل القيادة يدويًا",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2),
  },

  EventName.resumeRequired: {
    ET.WARNING: Alert(
      "اضغط Resume للخروج من التوقف",
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2),
  },

  EventName.belowSteerSpeed: {
    ET.WARNING: below_steer_speed_alert,
  },

  EventName.preLaneChangeLeft: {
    ET.WARNING: Alert(
      "وجّه لليسار لبدء تغيير المسار عند الأمان",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.preLaneChangeRight: {
    ET.WARNING: Alert(
      "وجّه لليمين لبدء تغيير المسار عند الأمان",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.laneChangeBlocked: {
    ET.WARNING: Alert(
      "تم رصد سيارة في المنطقة العمياء",
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, .1),
  },

  EventName.laneChange: {
    ET.WARNING: Alert(
      "جاري تغيير المسار",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.steerSaturated: {
    ET.WARNING: Alert(
      "تولَّ القيادة",
      "الانعطاف يتجاوز حد التوجيه",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.promptRepeat, 2.),
  },

  # Thrown when the fan is driven at >50% but is not rotating
  EventName.fanMalfunction: {
    ET.PERMANENT: NormalPermanentAlert("خلل في المروحة", "غالبًا مشكلة عتادية"),
  },

  # Camera is not outputting frames
  EventName.cameraMalfunction: {
    ET.PERMANENT: camera_malfunction_alert,
    ET.SOFT_DISABLE: soft_disable_alert("خلل في الكاميرا"),
    ET.NO_ENTRY: NoEntryAlert("خلل في الكاميرا: أعد تشغيل جهازك"),
  },
  # Camera framerate too low
  EventName.cameraFrameRate: {
    ET.PERMANENT: NormalPermanentAlert("معدل إطارات الكاميرا منخفض", "أعد تشغيل جهازك"),
    ET.SOFT_DISABLE: soft_disable_alert("معدل إطارات الكاميرا منخفض"),
    ET.NO_ENTRY: NoEntryAlert("معدل إطارات الكاميرا منخفض: أعد تشغيل جهازك"),
  },

  # Unused

  EventName.locationdTemporaryError: {
    ET.NO_ENTRY: NoEntryAlert("خطأ مؤقت في locationd"),
    ET.SOFT_DISABLE: soft_disable_alert("خطأ مؤقت في locationd"),
  },

  EventName.locationdPermanentError: {
    ET.NO_ENTRY: NoEntryAlert("خطأ دائم في locationd"),
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("خطأ دائم في locationd"),
    ET.PERMANENT: NormalPermanentAlert("خطأ دائم في locationd"),
  },

  # شرح تعلّم القائد الآلي محذوف من الواجهة (تعليق فقط)

  EventName.paramsdTemporaryError: {
    ET.NO_ENTRY: NoEntryAlert("خطأ مؤقت في paramsd"),
    ET.SOFT_DISABLE: soft_disable_alert("خطأ مؤقت في paramsd"),
  },

  EventName.paramsdPermanentError: {
    ET.NO_ENTRY: NoEntryAlert("خطأ دائم في paramsd"),
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("خطأ دائم في paramsd"),
    ET.PERMANENT: NormalPermanentAlert("خطأ دائم في paramsd"),
  },

  # ********** events that affect controls state transitions **********

  EventName.pcmEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.engage),
  },

  EventName.buttonEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.engage),
  },

  EventName.pcmDisable: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
  },

  EventName.buttonCancel: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("تم الضغط على إلغاء"),
  },

  EventName.brakeHold: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("تفعيل تثبيت الفرامل"),
  },

  EventName.parkBrake: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("فرامل التوقف مفعّلة"),
  },

  EventName.pedalPressed: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("تم الضغط على الدواسة",
                              visual_alert=VisualAlert.brakePressed),
  },

  EventName.preEnableStandstill: {
    ET.PRE_ENABLE: Alert(
      "ارفع قدمك عن الفرامل للتشغيل",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1, creation_delay=1.),
  },

  EventName.gasPressedOverride: {
    ET.OVERRIDE_LONGITUDINAL: Alert(
      "",
      "",
      AlertStatus.normal, AlertSize.none,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.steerOverride: {
    ET.OVERRIDE_LATERAL: Alert(
      "",
      "",
      AlertStatus.normal, AlertSize.none,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.wrongCarMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: wrong_car_mode_alert,
  },

  EventName.resumeBlocked: {
    ET.NO_ENTRY: NoEntryAlert("اضغط Set للتشغيل"),
  },

  EventName.wrongCruiseMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("تم تعطيل مثبت السرعة التكيفي"),
  },

  EventName.steerTempUnavailable: {
    ET.SOFT_DISABLE: soft_disable_alert("التوجيه غير متاح مؤقتًا"),
    ET.NO_ENTRY: NoEntryAlert("التوجيه غير متاح مؤقتًا"),
  },

  EventName.steerTimeLimit: {
    ET.SOFT_DISABLE: soft_disable_alert("الحد الزمني لتوجيه المركبة"),
    ET.NO_ENTRY: NoEntryAlert("الحد الزمني لتوجيه المركبة"),
  },

  EventName.outOfSpace: {
    ET.PERMANENT: out_of_space_alert,
    ET.NO_ENTRY: NoEntryAlert("نفادت المساحة التخزينية"),
  },

  EventName.belowEngageSpeed: {
    ET.NO_ENTRY: below_engage_speed_alert,
  },

  EventName.sensorDataInvalid: {
    ET.PERMANENT: Alert(
      "بيانات الحساسات غير صالحة",
      "ربما مشكلة عتادية",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("بيانات الحساسات غير صالحة"),
    ET.SOFT_DISABLE: soft_disable_alert("بيانات الحساسات غير صالحة"),
  },

  EventName.noGps: {
    ET.PERMANENT: Alert(
      "استقبال GPS ضعيف",
      "تأكد أن الجهاز يرى السماء بوضوح",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, .2, creation_delay=600.)
  },

  EventName.soundsUnavailable: {
    ET.PERMANENT: NormalPermanentAlert("السماعة غير موجودة", "أعد تشغيل جهازك"),
    ET.NO_ENTRY: NoEntryAlert("السماعة غير موجودة"),
  },

  EventName.tooDistracted: {
    ET.NO_ENTRY: NoEntryAlert("مستوى التشتت مرتفع جدًا"),
  },

  EventName.overheat: {
    ET.PERMANENT: overheat_alert,
    ET.SOFT_DISABLE: soft_disable_alert("ارتفاع حرارة النظام"),
    ET.NO_ENTRY: NoEntryAlert("ارتفاع حرارة النظام"),
  },

  EventName.wrongGear: {
    ET.SOFT_DISABLE: user_soft_disable_alert("ناقل الحركة ليس على D"),
    ET.NO_ENTRY: NoEntryAlert("ناقل الحركة ليس على D"),
  },

  EventName.calibrationInvalid: {
    ET.PERMANENT: calibration_invalid_alert,
    ET.SOFT_DISABLE: soft_disable_alert("المعايرة غير صالحة: أعد تركيب الجهاز وأعد المعايرة"),
    ET.NO_ENTRY: NoEntryAlert("المعايرة غير صالحة: أعد التركيب وأعد المعايرة"),
  },

  EventName.calibrationIncomplete: {
    ET.PERMANENT: calibration_incomplete_alert,
    ET.SOFT_DISABLE: soft_disable_alert("المعايرة غير مكتملة"),
    ET.NO_ENTRY: NoEntryAlert("المعايرة جارية"),
  },

  EventName.calibrationRecalibrating: {
    ET.PERMANENT: calibration_incomplete_alert,
    ET.SOFT_DISABLE: soft_disable_alert("تم رصد إعادة تركيب الجهاز: إعادة معايرة"),
    ET.NO_ENTRY: NoEntryAlert("تم رصد إعادة تركيب: إعادة معايرة"),
  },

  EventName.doorOpen: {
    ET.SOFT_DISABLE: user_soft_disable_alert("الباب مفتوح"),
    ET.NO_ENTRY: NoEntryAlert("الباب مفتوح"),
  },

  EventName.seatbeltNotLatched: {
    ET.SOFT_DISABLE: user_soft_disable_alert("حزام الأمان غير مُثبت"),
    ET.NO_ENTRY: NoEntryAlert("حزام الأمان غير مُثبت"),
  },

  EventName.espDisabled: {
    ET.SOFT_DISABLE: soft_disable_alert("تم تعطيل مانع الانزلاق الإلكتروني"),
    ET.NO_ENTRY: NoEntryAlert("تم تعطيل مانع الانزلاق الإلكتروني"),
  },

  EventName.lowBattery: {
    ET.SOFT_DISABLE: soft_disable_alert("البطارية منخفضة"),
    ET.NO_ENTRY: NoEntryAlert("البطارية منخفضة"),
  },

  EventName.commIssue: {
    ET.SOFT_DISABLE: soft_disable_alert("مشكلة اتصال بين العمليات"),
    ET.NO_ENTRY: comm_issue_alert,
  },
  EventName.commIssueAvgFreq: {
    ET.SOFT_DISABLE: soft_disable_alert("معدل الاتصال بين العمليات منخفض"),
    ET.NO_ENTRY: NoEntryAlert("معدل الاتصال بين العمليات منخفض"),
  },

  EventName.controlsdLagging: {
    ET.SOFT_DISABLE: soft_disable_alert("عملية التحكم متأخرة"),
    ET.NO_ENTRY: NoEntryAlert("عملية التحكم متأخرة: أعد تشغيل جهازك"),
  },

  EventName.processNotRunning: {
    ET.NO_ENTRY: process_not_running_alert,
    ET.SOFT_DISABLE: soft_disable_alert("العملية غير قيد التشغيل"),
  },

  EventName.radarFault: {
    ET.SOFT_DISABLE: soft_disable_alert("خطأ في الرادار: أعد تشغيل السيارة"),
    ET.NO_ENTRY: NoEntryAlert("خطأ في الرادار: أعد تشغيل السيارة"),
  },

  EventName.modeldLagging: {
    ET.SOFT_DISABLE: soft_disable_alert("نموذج القيادة متأخر"),
    ET.NO_ENTRY: NoEntryAlert("نموذج القيادة متأخر"),
    ET.PERMANENT: modeld_lagging_alert,
  },

  EventName.posenetInvalid: {
    ET.SOFT_DISABLE: soft_disable_alert("سرعة Posenet غير صالحة"),
    ET.NO_ENTRY: posenet_invalid_alert,
  },

  EventName.deviceFalling: {
    ET.SOFT_DISABLE: soft_disable_alert("سقوط الجهاز من الحامل"),
    ET.NO_ENTRY: NoEntryAlert("سقوط الجهاز من الحامل"),
  },

  EventName.lowMemory: {
    ET.SOFT_DISABLE: soft_disable_alert("ذاكرة منخفضة: أعد تشغيل جهازك"),
    ET.PERMANENT: low_memory_alert,
    ET.NO_ENTRY: NoEntryAlert("ذاكرة منخفضة: أعد تشغيل جهازك"),
  },

  EventName.highCpuUsage: {
    #ET.SOFT_DISABLE: soft_disable_alert("خلل بالنظام: أعد تشغيل جهازك"),
    #ET.PERMANENT: NormalPermanentAlert("خلل بالنظام", "أعد تشغيل جهازك"),
    ET.NO_ENTRY: high_cpu_usage_alert,
  },

  EventName.accFaulted: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("عطل في المثبت: أعد تشغيل السيارة"),
    ET.PERMANENT: NormalPermanentAlert("عطل في المثبت: أعد تشغيل السيارة للتشغيل"),
    ET.NO_ENTRY: NoEntryAlert("عطل في المثبت: أعد تشغيل السيارة"),
  },

  EventName.controlsMismatch: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("عدم تطابق في التحكم"),
    ET.NO_ENTRY: NoEntryAlert("عدم تطابق في التحكم"),
  },

  EventName.roadCameraError: {
    ET.PERMANENT: NormalPermanentAlert("خطأ CRC في الكاميرا - الطريق",
                                       duration=1.,
                                       creation_delay=30.),
  },

  EventName.wideRoadCameraError: {
    ET.PERMANENT: NormalPermanentAlert("خطأ CRC في الكاميرا - طريق عين السمكة",
                                       duration=1.,
                                       creation_delay=30.),
  },

  EventName.driverCameraError: {
    ET.PERMANENT: NormalPermanentAlert("خطأ CRC في الكاميرا - السائق",
                                       duration=1.,
                                       creation_delay=30.),
  },

  EventName.usbError: {
    ET.SOFT_DISABLE: soft_disable_alert("خطأ USB: أعد تشغيل جهازك"),
    ET.PERMANENT: NormalPermanentAlert("خطأ USB: أعد تشغيل جهازك", ""),
    ET.NO_ENTRY: NoEntryAlert("خطأ USB: أعد تشغيل جهازك"),
  },

  EventName.canError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("خطأ CAN"),
    ET.PERMANENT: Alert(
      "خطأ CAN: تحقق من التوصيلات",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 1., creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("خطأ CAN: تحقق من التوصيلات"),
  },

  EventName.canBusMissing: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("فصل ناقل CAN"),
    ET.PERMANENT: Alert(
      "فصل ناقل CAN: سلك معطوب غالبًا",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 1., creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("فصل ناقل CAN: تحقق من التوصيلات"),
  },

  EventName.steerUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("عطل LKAS: أعد تشغيل السيارة"),
    ET.PERMANENT: NormalPermanentAlert("عطل LKAS: أعد تشغيل السيارة للتشغيل"),
    ET.NO_ENTRY: NoEntryAlert("عطل LKAS: أعد تشغيل السيارة"),
  },

  EventName.reverseGear: {
    ET.PERMANENT: Alert(
      "تعشيق\nالخلف",
      "",
      AlertStatus.normal, AlertSize.full,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .2, creation_delay=0.5),
    ET.USER_DISABLE: ImmediateDisableAlert("تعشيق الخلف"),
    ET.NO_ENTRY: NoEntryAlert("تعشيق الخلف"),
  },

  EventName.cruiseDisabled: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("تم إطفاء المثبت"),
  },

  EventName.relayMalfunction: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("خلل في مرحّل الضفيرة"),
    ET.PERMANENT: NormalPermanentAlert("خلل في مرحّل الضفيرة", "تحقق من العتاد"),
    ET.NO_ENTRY: NoEntryAlert("خلل في مرحّل الضفيرة"),
  },

  EventName.speedTooLow: {
    ET.IMMEDIATE_DISABLE: Alert(
      "تم إلغاء القائد الآلي",
      "السرعة منخفضة جدًا",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.disengage, 3.),
  },

  EventName.speedTooHigh: {
    ET.WARNING: Alert(
      "السرعة عالية جدًا",
      "النموذج غير واثق عند هذه السرعة",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.promptRepeat, 4.),
    ET.NO_ENTRY: NoEntryAlert("خفّف السرعة للتشغيل"),
  },

  EventName.lowSpeedLockout: {
    ET.PERMANENT: NormalPermanentAlert("عطل في المثبت: أعد تشغيل السيارة للتشغيل"),
    ET.NO_ENTRY: NoEntryAlert("عطل في المثبت: أعد تشغيل السيارة"),
  },

  EventName.lkasDisabled: {
    ET.PERMANENT: NormalPermanentAlert("تم تعطيل LKAS: فعّل LKAS للتشغيل"),
    ET.NO_ENTRY: NoEntryAlert("تم تعطيل LKAS"),
  },

  EventName.vehicleSensorsInvalid: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("حساسات المركبة غير صالحة"),
    ET.PERMANENT: NormalPermanentAlert("معايرة حساسات المركبة", "قد للتعيير"),
    ET.NO_ENTRY: NoEntryAlert("معايرة حساسات المركبة"),
  },

  # FrogPilot Events (نصوص مرئية)
  EventName.blockUser: {
    ET.PERMANENT: Alert(
      "لا تستخدم فرع 'Development'!",
      "سيتم إدخالك في 'وضع الداش كام' لسلامتك",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.none, 1.),
  },

  EventName.customStartupAlert: {
    ET.PERMANENT: custom_startup_alert,
  },

  EventName.forcingStop: {
    ET.WARNING: forcing_stop_alert,
  },

  EventName.goatSteerSaturated: {
    ET.WARNING: Alert(
      "خُذها يا يسوع!!",
      "الانعطاف يتجاوز حد التوجيه",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.goat, 2.),
  },

  EventName.greenLight: {
    ET.PERMANENT: Alert(
      "الإشارة صارت خضراء",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.MID, VisualAlert.none, AudibleAlert.prompt, 3.),
  },

  EventName.holidayActive: {
    ET.PERMANENT: holiday_alert,
  },

  EventName.laneChangeBlockedLoud: {
    ET.WARNING: Alert(
      "تم رصد سيارة في المنطقة العمياء",
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.warningSoft, .1),
  },

  EventName.leadDeparting: {
    ET.PERMANENT: Alert(
      "المركبة الأمامية تحرّكت",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.MID, VisualAlert.none, AudibleAlert.prompt, 3.),
  },

  EventName.noLaneAvailable: {
    ET.WARNING: no_lane_available_alert,
  },

  EventName.openpilotCrashed: {
    ET.IMMEDIATE_DISABLE: Alert(
      "تعطّل القائد الآلي",
      "فضلاً انشر 'سجلّ الخطأ' في ديسكورد القائد الآلي!",
      AlertStatus.critical, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.prompt, .1),

    ET.NO_ENTRY: Alert(
      "تعطّل القائد الآلي",
      "فضلاً انشر 'سجلّ الخطأ' في ديسكورد القائد الآلي!",
      AlertStatus.critical, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.prompt, .1),
  },

  EventName.pedalInterceptorNoBrake: {
    ET.WARNING: Alert(
      "الفرملة غير متاحة",
      "حوّل إلى L",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.HIGH, VisualAlert.wrongGear, AudibleAlert.promptRepeat, 4.),
  },

  EventName.speedLimitChanged: {
    ET.PERMANENT: Alert(
      "تم تغيير حد السرعة",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, 3.),
  },

  EventName.thisIsFineSteerSaturated: {
    ET.WARNING: Alert(
      "كل شيء بخير ☕",
      "الانعطاف يتجاوز حد التوجيه",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.thisIsFine, 2.),
  },

  EventName.torqueNNLoad: {
    ET.PERMANENT: torque_nn_load_alert,
  },

  EventName.trafficModeActive: {
    ET.WARNING: Alert(
      "تم تفعيل وضع الازدحام",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, 3.),
  },

  EventName.trafficModeInactive: {
    ET.WARNING: Alert(
      "تم إلغاء تفعيل وضع الازدحام",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, 3.),
  },

  EventName.turningLeft: {
    ET.WARNING: Alert(
      "انعطاف يسار",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.turningRight: {
    ET.WARNING: Alert(
      "انعطاف يمين",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  # Random Events
  EventName.accel30: {
    ET.WARNING: Alert(
      "هاه شوي مسرّع!",
      "(⁄ ⁄•⁄ω⁄•⁄ ⁄)",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.uwu, 4.),
  },

  EventName.accel35: {
    ET.WARNING: Alert(
      "ما أعطيك Tree-Fiddy!",
      "يا وحش لوخ نِس!",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.nessie, 4.),
  },

  EventName.accel40: {
    ET.WARNING: Alert(
      "يا ساتر!",
      "🚗💨",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.doc, 4.),
  },

  EventName.dejaVuCurve: {
    ET.PERMANENT: Alert(
      "♬♪ ديجا فو! ᕕ(⌐■_■)ᕗ ♪♬",
      "🏎️",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.dejaVu, 4.),
  },

  EventName.firefoxSteerSaturated: {
    ET.WARNING: Alert(
      "انتهى دعم Internet Explorer...",
      "الانعطاف يتجاوز حد التوجيه",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.firefox, 4.),
  },

  EventName.hal9000: {
    ET.WARNING: Alert(
      "عذرًا يا ديف",
      "أخشى أني لا أستطيع فعل ذلك...",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.hal9000, 4.),
  },

  EventName.openpilotCrashedRandomEvent: {
    ET.IMMEDIATE_DISABLE: Alert(
      "تعطّل القائد الآلي 💩",
      "فضلاً انشر 'سجلّ الخطأ' في ديسكورد القائد الآلي!",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.fart, 10.),

    ET.NO_ENTRY: Alert(
      "تعطّل القائد الآلي 💩",
      "فضلاً انشر 'سجلّ الخطأ' في ديسكورد القائد الآلي!",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.fart, 10.),
  },

  EventName.toBeContinued: {
    ET.PERMANENT: Alert(
      "يتبع...",
      "⬅️",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.MID, VisualAlert.none, AudibleAlert.continued, 7.),
  },

  EventName.vCruise69: {
    ET.WARNING: Alert(
      "لول 69",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.noice, 2.),
  },

  EventName.yourFrogTriedToKillMe: {
    ET.PERMANENT: Alert(
      "الضفدع حاول يقتلني...",
      "👺",
      AlertStatus.frogpilot, AlertSize.mid,
      Priority.MID, VisualAlert.none, AudibleAlert.angry, 5.),
  },

  EventName.youveGotMail: {
    ET.WARNING: Alert(
      "وصلك بريد! 📧",
      "",
      AlertStatus.frogpilot, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.mail, 3.),
  },
}

if __name__ == '__main__':
  # print all alerts by type and priority
  from cereal.services import SERVICE_LIST
  from collections import defaultdict

  event_names = {v: k for k, v in EventName.schema.enumerants.items()}
  alerts_by_type: dict[str, dict[Priority, list[str]]] = defaultdict(lambda: defaultdict(list))

  CP = car.CarParams.new_message()
  CS = car.CarState.new_message()
  sm = messaging.SubMaster(list(SERVICE_LIST.keys()))

  for i, alerts in EVENTS.items():
    for et, alert in alerts.items():
      if callable(alert):
        alert = alert(CP, CS, sm, False, 1)
      alerts_by_type[et][alert.priority].append(event_names[i])

  all_alerts: dict[str, list[tuple[Priority, list[str]]]] = {}
  for et, priority_alerts in alerts_by_type.items():
    all_alerts[et] = sorted(priority_alerts.items(), key=lambda x: x[0], reverse=True)

  for status, evs in sorted(all_alerts.items(), key=lambda x: x[0]):
    print(f"**** {status} ****")
    for p, alert_list in evs:
      print(f"  {repr(p)}:")
      print("   ", ', '.join(alert_list), "\n")
