#include "cn105_packet.h"
#include <cmath>
#include <cstring>

namespace mit {

uint8_t computeChecksum(const uint8_t *buf, size_t len) {
  uint32_t sum = 0;
  for (size_t i = 0; i < len; ++i)
    sum += buf[i];
  return static_cast<uint8_t>((0xFC - sum) & 0xFF);
}

static void writeStandardHeader(uint8_t *out, uint8_t type) {
  out[0] = HEADER_BYTE;
  out[1] = type;
  out[2] = ADDR1;
  out[3] = ADDR2;
  out[4] = static_cast<uint8_t>(DATA_LEN_STANDARD);
}

size_t buildConnect(uint8_t out[8]) {
  out[0] = HEADER_BYTE;
  out[1] = TYPE_CONNECT;
  out[2] = ADDR1;
  out[3] = ADDR2;
  out[4] = static_cast<uint8_t>(DATA_LEN_CONNECT);
  out[5] = CONNECT_CHALLENGE_0;
  out[6] = CONNECT_CHALLENGE_1;
  out[7] = computeChecksum(out, 7);
  return 8;
}

size_t buildInfoRequest(uint8_t out[PACKET_LEN_STANDARD], uint8_t info_mode) {
  std::memset(out, 0, PACKET_LEN_STANDARD);
  writeStandardHeader(out, TYPE_GET);
  // Data section starts at index 5; first byte of data = info_mode
  out[PACKET_HEADER_LEN + 0] = info_mode;
  out[PACKET_LEN_STANDARD - 1] = computeChecksum(out, PACKET_LEN_STANDARD - 1);
  return PACKET_LEN_STANDARD;
}

// Temperature byte (new encoding): byte = round(temp * 2.0f) + 128.
// Range tested 16..31°C maps to 0xA0..0xBE.
static uint8_t encodeTempNew(float t) {
  int v = static_cast<int>(std::lround(t * 2.0f)) + 128;
  if (v < 0)
    v = 0;
  if (v > 255)
    v = 255;
  return static_cast<uint8_t>(v);
}

// Legacy temperature byte: 0=31°C .. 0x0F=16°C. Used as fallback for older
// units.
static uint8_t encodeTempLegacy(float t) {
  int v = 31 - static_cast<int>(std::lround(t));
  if (v < 0)
    v = 0;
  if (v > 0x0F)
    v = 0x0F;
  return static_cast<uint8_t>(v);
}

size_t buildSetPacket(uint8_t out[PACKET_LEN_STANDARD],
                      const PendingSettings &p) {
  // Reference uses HEADER[8] = { 0xFC, 0x41, 0x01, 0x30, 0x10, 0x01, 0x00, 0x00
  // } i.e. the SET sub-type marker is HEADER[5] = 0x01, then flags1/flags2 live
  // at packet[6]/[7] (initially 0), and all other bytes are zero. Only fields
  // whose flag bit is set should be written — the heatpump appears to validate
  // that.
  std::memset(out, 0, PACKET_LEN_STANDARD);
  writeStandardHeader(out, TYPE_SET);
  out[5] = 0x01; // SET sub-type marker (HEADER[5])
  out[6] = p.flags1;
  out[7] = p.flags2;
  if (p.flags1 & FLAG1_POWER)
    out[8] = p.power ? 1 : 0;
  if (p.flags1 & FLAG1_MODE)
    out[9] = p.mode;
  if (p.flags1 & FLAG1_TEMP) {
    out[10] = encodeTempLegacy(p.targetC);
    out[19] = encodeTempNew(p.targetC);
  }
  if (p.flags1 & FLAG1_FAN)
    out[11] = p.fan;
  if (p.flags1 & FLAG1_VANE)
    out[12] = p.vaneV;
  if (p.flags2 & FLAG2_WVANE)
    out[18] = p.wideVane;

  out[PACKET_LEN_STANDARD - 1] = computeChecksum(out, PACKET_LEN_STANDARD - 1);
  return PACKET_LEN_STANDARD;
}

bool parseFrame(const uint8_t *buf, size_t len, ParsedFrame &out) {
  if (len < PACKET_HEADER_LEN + 1)
    return false;
  if (buf[0] != HEADER_BYTE)
    return false;

  const uint8_t lenField = buf[4];
  const size_t expected = PACKET_HEADER_LEN + static_cast<size_t>(lenField) + 1;
  if (len != expected)
    return false;

  const uint8_t calc = computeChecksum(buf, len - 1);
  if (calc != buf[len - 1])
    return false;

  out.type = buf[1];
  out.data = buf + PACKET_HEADER_LEN;
  out.dataLen = lenField;
  return true;
}

// ---- Response parsers -----------------------------------------------------
// Data layout (from MitsubishiCN105ESPHome reference):
//  - SETTINGS (info=0x02):
//      data[0]=info_mode=0x02
//      data[3]=power, data[4]=mode (bit3 = iSee flag), data[5]=temp-legacy,
//      data[6]=fan, data[7]=vaneV,
//      data[10]=wideVane, data[11]=temp-new
//  - ROOM_TEMP (info=0x03):
//      data[0]=0x03
//      data[3]=temp-legacy fallback, data[6]=temp-new (preferred when !=0)
//  - STATUS (info=0x06):
//      data[0]=0x06
//      data[3]=compressorFrequency (only valid if operating)
//      data[4]=operating (1=running, 0=standby)

bool applySettingsResponse(const uint8_t *data, size_t dataLen, State &s) {
  if (dataLen < 12)
    return false;
  bool changed = false;

  bool power = (data[3] & 0x01) != 0;
  if (power != s.power) {
    s.power = power;
    changed = true;
  }

  uint8_t modeRaw =
      static_cast<uint8_t>(data[4] & 0x07); // strip iSee flag (bit3)
  if (modeRaw != s.modeRaw) {
    s.modeRaw = modeRaw;
    changed = true;
  }

  uint8_t fan = data[6];
  if (fan != s.fanRaw) {
    s.fanRaw = fan;
    changed = true;
  }

  uint8_t vaneV = data[7];
  if (vaneV != s.vaneVRaw) {
    s.vaneVRaw = vaneV;
    changed = true;
  }

  uint8_t wideVane = data[10] & 0x0F;
  if (wideVane != s.wideVaneRaw) {
    s.wideVaneRaw = wideVane;
    changed = true;
  }

  float target;
  if (data[11] != 0) {
    target = (static_cast<int>(data[11]) - 128) / 2.0f;
  } else {
    target = 31.0f - static_cast<float>(data[5] & 0x0F);
  }
  if (target != s.targetC) {
    s.targetC = target;
    changed = true;
  }

  s.haveSettings = true;
  return changed;
}

bool applyRoomTempResponse(const uint8_t *data, size_t dataLen, State &s) {
  if (dataLen < 7)
    return false;
  float roomC;
  if (data[6] != 0) {
    roomC = (static_cast<int>(data[6]) - 128) / 2.0f;
  } else {
    // Legacy lookup: data[3] in [0..31] maps to 10..41 (1°C step).
    // This is the reference's ROOM_TEMP_MAP[i] = 10 + i.
    uint8_t idx = data[3];
    if (idx > 31)
      idx = 31;
    roomC = 10.0f + static_cast<float>(idx);
  }
  bool changed = (roomC != s.roomC) || !s.haveRoomTemp;
  s.roomC = roomC;
  s.haveRoomTemp = true;
  return changed;
}

bool applyStatusResponse(const uint8_t *data, size_t dataLen, State &s) {
  if (dataLen < 5)
    return false;
  bool operating = (data[4] != 0);
  uint8_t hz = operating ? data[3] : 0;
  bool changed = (operating != s.operating) || (hz != s.compressorHz);
  s.operating = operating;
  s.compressorHz = hz;
  return changed;
}

} // namespace mit
