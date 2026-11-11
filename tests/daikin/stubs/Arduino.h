#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
inline uint32_t test_now = 100;
inline uint32_t millis() { return test_now; }
struct HardwareSerial { template<class... T> void printf(const char*, T...) {} void println(const char*) {} };
inline HardwareSerial Serial, Serial1;
