#pragma once

#include "../../AirConditionDriver.h"
#include <cstddef>
#include <cstdint>

namespace mit {

// ---- Frame constants -------------------------------------------------------
constexpr uint8_t HEADER_BYTE = 0xFC;
constexpr uint8_t TYPE_SET = 0x41;
constexpr uint8_t TYPE_GET = 0x42;
constexpr uint8_t TYPE_CONNECT = 0x5A;
constexpr uint8_t TYPE_SET_RESP = 0x61;
constexpr uint8_t TYPE_GET_RESP = 0x62;
constexpr uint8_t TYPE_CONNECT_RESP = 0x7A;

constexpr uint8_t ADDR1 = 0x01;
constexpr uint8_t ADDR2 = 0x30;

// Standard packet layout: HEADER(5) + DATA(LEN) + CHECKSUM(1)
constexpr size_t PACKET_HEADER_LEN = 5;
constexpr size_t DATA_LEN_STANDARD = 16; // SET/GET data payload
constexpr size_t PACKET_LEN_STANDARD =
    PACKET_HEADER_LEN + DATA_LEN_STANDARD + 1; // 22

constexpr size_t DATA_LEN_CONNECT = 2;
constexpr size_t PACKET_LEN_CONNECT =
    PACKET_HEADER_LEN + DATA_LEN_CONNECT + 1; // 8 + checksum at idx 7

// ---- Info modes (byte 5 of GET / first byte of GET response payload) ------
constexpr uint8_t INFO_SETTINGS = 0x02;
constexpr uint8_t INFO_ROOM_TEMP = 0x03;
constexpr uint8_t INFO_TIMERS = 0x05;
constexpr uint8_t INFO_STATUS = 0x06;
constexpr uint8_t INFO_STANDBY = 0x09;

// ---- SET packet flag bits (bytes 6 and 7 of data) -------------------------
constexpr uint8_t FLAG1_POWER = 0x01;
constexpr uint8_t FLAG1_MODE = 0x02;
constexpr uint8_t FLAG1_TEMP = 0x04;
constexpr uint8_t FLAG1_FAN = 0x08;
constexpr uint8_t FLAG1_VANE = 0x10;
constexpr uint8_t FLAG2_WVANE = 0x01;

// ---- Mode codes -----------------------------------------------------------
constexpr uint8_t MODE_HEAT = 0x01;
constexpr uint8_t MODE_DRY = 0x02;
constexpr uint8_t MODE_COOL = 0x03;
constexpr uint8_t MODE_FAN = 0x07;
constexpr uint8_t MODE_AUTO = 0x08;

// ---- Fan codes ------------------------------------------------------------
// CN105 fan steps: AUTO, QUIET, 1, 2, 3, 4
constexpr uint8_t FAN_AUTO = 0x00;
constexpr uint8_t FAN_QUIET = 0x01;
constexpr uint8_t FAN_1 = 0x02;
constexpr uint8_t FAN_2 = 0x03;
constexpr uint8_t FAN_3 = 0x05;
constexpr uint8_t FAN_4 = 0x06;

// ---- Vane vertical codes --------------------------------------------------
constexpr uint8_t VANE_V_AUTO = 0x00;
constexpr uint8_t VANE_V_POS1 = 0x01; // ↑↑
constexpr uint8_t VANE_V_POS2 = 0x02; // ↑
constexpr uint8_t VANE_V_POS3 = 0x03; // —
constexpr uint8_t VANE_V_POS4 = 0x04; // ↓
constexpr uint8_t VANE_V_POS5 = 0x05; // ↓↓
constexpr uint8_t VANE_V_SWING = 0x07;

// ---- Vane horizontal (wide vane) codes -----------------------------------
constexpr uint8_t WVANE_LL = 0x01;  // ←←
constexpr uint8_t WVANE_L = 0x02;   // ←
constexpr uint8_t WVANE_MID = 0x03; // |
constexpr uint8_t WVANE_R = 0x04;   // →
constexpr uint8_t WVANE_RR = 0x05;  // →→
constexpr uint8_t WVANE_LR = 0x08;  // ←→
constexpr uint8_t WVANE_SWING = 0x0C;

// ---- Connect challenge (verbatim from reference) -------------------------
constexpr uint8_t CONNECT_CHALLENGE_0 = 0xCA;
constexpr uint8_t CONNECT_CHALLENGE_1 = 0x01;

// ---- Driver state ---------------------------------------------------------
struct State {
  bool power{false};
  uint8_t modeRaw{MODE_AUTO};
  float targetC{22.0f};
  float roomC{20.0f};
  uint8_t fanRaw{FAN_AUTO};
  uint8_t vaneVRaw{VANE_V_AUTO};
  uint8_t wideVaneRaw{WVANE_MID};
  bool operating{false};   // STATUS info: compressor running
  uint8_t compressorHz{0}; // not surfaced in v1 but tracked
  bool haveSettings{false};
  bool haveRoomTemp{false};
};

// Tracks which fields a SET packet should update (flags1/flags2 encode this)
struct PendingSettings {
  uint8_t flags1{0};
  uint8_t flags2{0};
  bool power{false};
  uint8_t mode{MODE_AUTO};
  float targetC{22.0f};
  uint8_t fan{FAN_AUTO};
  uint8_t vaneV{VANE_V_AUTO};
  uint8_t wideVane{WVANE_MID};
};

struct Stats {
  uint32_t framesOk{0};
  uint32_t framesBadChecksum{0};
  uint32_t framesBadFormat{0};
  uint32_t timeouts{0};
  uint32_t connectsSent{0};
};

// ---- OpenKNX <-> CN105 mode conversion -----------------------------------
inline uint8_t openknx_to_mit_mode(AirConditionMode m) {
  switch (m) {
  case AirConditionModeHeat:
    return MODE_HEAT;
  case AirConditionModeCool:
    return MODE_COOL;
  case AirConditionModeDry:
    return MODE_DRY;
  case AirConditionModeFan:
    return MODE_FAN;
  case AirConditionModeAuto:
    return MODE_AUTO;
  default:
    return MODE_AUTO;
  }
}

inline AirConditionMode mit_to_openknx_mode(uint8_t m) {
  switch (m) {
  case MODE_HEAT:
    return AirConditionModeHeat;
  case MODE_COOL:
    return AirConditionModeCool;
  case MODE_DRY:
    return AirConditionModeDry;
  case MODE_FAN:
    return AirConditionModeFan;
  case MODE_AUTO:
  default:
    return AirConditionModeAuto;
  }
}

// OpenKNX fan: 0=auto, 1..N stepped. We expose 5 user steps
// (1=quiet, 2..5=Mitsubishi 1..4)
inline uint8_t openknx_to_mit_fan(unsigned int speed) {
  switch (speed) {
  case 0:
    return FAN_AUTO;
  case 1:
    return FAN_QUIET;
  case 2:
    return FAN_1;
  case 3:
    return FAN_2;
  case 4:
    return FAN_3;
  case 5:
    return FAN_4;
  default:
    return FAN_AUTO;
  }
}

inline unsigned int mit_to_openknx_fan(uint8_t fan) {
  switch (fan) {
  case FAN_AUTO:
    return 0;
  case FAN_QUIET:
    return 1;
  case FAN_1:
    return 2;
  case FAN_2:
    return 3;
  case FAN_3:
    return 4;
  case FAN_4:
    return 5;
  default:
    return 0;
  }
}

// OpenKNX vane fix position 1..5 maps directly to Mitsubishi VANE_V_POS1..5
inline uint8_t openknx_to_mit_vane_v(unsigned int pos) {
  if (pos >= 1 && pos <= 5)
    return static_cast<uint8_t>(pos);
  return VANE_V_AUTO;
}

inline unsigned int mit_to_openknx_vane_v_pos(uint8_t v) {
  if (v >= VANE_V_POS1 && v <= VANE_V_POS5)
    return v;
  return 0;
}

inline uint8_t openknx_to_mit_wide_vane(unsigned int pos) {
  switch (pos) {
  case 1:
    return WVANE_LL;
  case 2:
    return WVANE_L;
  case 3:
    return WVANE_MID;
  case 4:
    return WVANE_R;
  case 5:
    return WVANE_RR;
  default:
    return WVANE_MID;
  }
}

inline unsigned int mit_to_openknx_wide_vane_pos(uint8_t w) {
  switch (w) {
  case WVANE_LL:
    return 1;
  case WVANE_L:
    return 2;
  case WVANE_MID:
    return 3;
  case WVANE_R:
    return 4;
  case WVANE_RR:
    return 5;
  default:
    return 0;
  }
}

inline bool is_vane_v_swing(uint8_t v) { return v == VANE_V_SWING; }
inline bool is_wide_vane_swing(uint8_t w) {
  return w == WVANE_SWING || w == WVANE_LR;
}

} // namespace mit
