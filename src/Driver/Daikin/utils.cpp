#include <cstdio>
#include <string>
#include "utils.h"

namespace daikin {

// OpenKNX-compatible hex representation function
std::string hex_repr(std::span<const uint8_t> bytes) {
  std::string result;
  result.reserve(bytes.size() * 3); // 2 hex chars + separator per byte
  
  for (size_t i = 0; i < bytes.size(); ++i) {
    if (i > 0) result += ':';
    char buf[3];
    snprintf(buf, sizeof(buf), "%02X", bytes[i]);
    result += buf;
  }
  return result;
}

// UART debugger
std::string str_repr(std::span<const uint8_t> bytes) {
  std::string res;
  char buf[5];
  for (const auto b : bytes) {
    if (b == 7) {
      res += "\\a";
    } else if (b == 8) {
      res += "\\b";
    } else if (b == 9) {
      res += "\\t";
    } else if (b == 10) {
      res += "\\n";
    } else if (b == 11) {
      res += "\\v";
    } else if (b == 12) {
      res += "\\f";
    } else if (b == 13) {
      res += "\\r";
    } else if (b == 27) {
      res += "\\e";
    } else if (b == 34) {
      res += "\\\"";
    } else if (b == 39) {
      res += "\\'";
    } else if (b == 92) {
      res += "\\\\";
    } else if (b < 32 || b > 127) {
      sprintf(buf, "\\x%02X", b);
      res += buf;
    } else {
      res += b;
    }
  }
  return res;
}

} // namespace daikin
