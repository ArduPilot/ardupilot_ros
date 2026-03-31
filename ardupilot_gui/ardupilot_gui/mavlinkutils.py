"""
Helper functions from pymavlink
"""
from pymavlink import mavutil


def mode_string(vehtype, mode):
    """Return the text string name of a flight mode"""
    mode_map = mode_mapping_bynumber(vehtype)
    if mode_map and mode in mode_map:
        return mode_map[mode]
    return "Mode(%u)" % mode


def mode_int(vehtype, modestr):
    """Return the mode number for a given mode string, or None if not found"""
    mode_map = mode_mapping_bynumber(vehtype)
    if mode_map:
        for k, v in mode_map.items():
            if v == modestr:
                return k
    return None


def mode_mapping_bynumber(mav_type):
    """return dictionary mapping mode numbers to name, or None if unknown"""
    return AP_MAV_TYPE_MODE_MAP[mav_type] if mav_type in AP_MAV_TYPE_MODE_MAP else None


FAILSAFE_ID = {
    21: "Radio ",
    22: "Battery ",
    23: "GCS ",
    24: "EKF ",
}

VEHICLE_TYPES = {
    0: "Generic",
    1: "Rover",
    2: "Copter",
    3: "Plane",
    4: "Antenna Tracker",
    5: "Unknown",
    6: "Replay",
    7: "Sub",
    8: "IOFirmware",
    9: "Peripth",
    10: "DAL Standalone",
    11: "Bootloader",
    12: "Blimp",
    13: "Heli",
}

AP_MAV_TYPE_MODE_MAP = {
    1: mavutil.mode_mapping_rover,
    2: mavutil.mode_mapping_acm,
    3: mavutil.mode_mapping_apm,
    4: mavutil.mode_mapping_tracker,
    7: mavutil.mode_mapping_sub,
    12: mavutil.mode_mapping_blimp,
    13: mavutil.mode_mapping_acm,  # Helicopter uses same modes as APM Copter
}
