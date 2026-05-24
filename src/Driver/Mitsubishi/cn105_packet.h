#pragma once

#include "cn105_types.h"

namespace mit {

struct ParsedFrame {
  uint8_t type{0}; // 0x61 / 0x62 / 0x7A
  const uint8_t *data{
      nullptr}; // pointer into the source buffer (data section only)
  size_t dataLen{0};
};

// Sum-based checksum: (0xFC - sum(bytes[0..len-1])) & 0xFF
uint8_t computeChecksum(const uint8_t *buf, size_t len);

// Returns total bytes written (8). Out buffer must be at least 8 bytes.
size_t buildConnect(uint8_t out[8]);

// Returns total bytes written (PACKET_LEN_STANDARD = 22).
size_t buildInfoRequest(uint8_t out[PACKET_LEN_STANDARD], uint8_t info_mode);

// Returns total bytes written (PACKET_LEN_STANDARD = 22).
size_t buildSetPacket(uint8_t out[PACKET_LEN_STANDARD],
                      const PendingSettings &p);

// Validates frame header, length, checksum. On success fills `out` and returns
// true. `len` is the total bytes including header and checksum.
bool parseFrame(const uint8_t *buf, size_t len, ParsedFrame &out);

// Returns expected total frame length (header+data+checksum) for given
// length-field byte.
inline size_t expectedFrameLen(uint8_t lenField) {
  return PACKET_HEADER_LEN + static_cast<size_t>(lenField) + 1;
}

// Apply parsed GET responses to state. Returns true if anything changed.
bool applySettingsResponse(const uint8_t *data, size_t dataLen, State &s);
bool applyRoomTempResponse(const uint8_t *data, size_t dataLen, State &s);
bool applyStatusResponse(const uint8_t *data, size_t dataLen, State &s);

} // namespace mit
