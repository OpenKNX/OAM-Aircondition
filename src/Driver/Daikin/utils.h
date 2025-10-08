#pragma once

#include <cstdint>
#include <string>
#include <span>

namespace daikin {

// printf format specifier macros for std::string_view
#define PRI_SV ".*s"
#define PRI_SV_ARGS(x) (x).size(), (x).data()

std::string hex_repr(std::span<const uint8_t> bytes);
std::string str_repr(std::span<const uint8_t> bytes);

static constexpr uint8_t ahex_digit(const uint8_t digit) { return (digit >= 'A') ? (digit - 'A') + 10 : digit - '0'; }
static constexpr uint8_t ahex_u8_le(const uint8_t first, const uint8_t second) { return ahex_digit(first) + (ahex_digit(second) << 4); }
static constexpr uint16_t ahex_u16_le(std::span<const uint8_t> bytes) { return ahex_u8_le(bytes[0], bytes[1]) + (ahex_u8_le(bytes[2], bytes[3]) << 8); }

} // namespace daikin
