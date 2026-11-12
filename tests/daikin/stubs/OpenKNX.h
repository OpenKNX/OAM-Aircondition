#pragma once
#include "Arduino.h"
class GroupObject {};
#include <cstdarg>
inline std::vector<std::string> test_messages;
inline void test_log(const char* format, ...) __attribute__((format(printf, 1, 2)));
inline void test_log(const char* format, ...)
{
    char message[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    test_messages.emplace_back(message);
}
#define logInfoP(...) test_log(__VA_ARGS__)
#define logErrorP(...) ((void)0)
#define logDebugP(...) ((void)0)
