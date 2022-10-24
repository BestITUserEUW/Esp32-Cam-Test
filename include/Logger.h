#pragma once

#include <string.h>
#include <HardwareSerial.h>

#define FMT_FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define LOGGER_FUNCTION static_cast<const char*>(__FUNCTION__)

#define LOG_LEVEL_DEBUG 0x00
#define LOG_LEVEL_INFO 0x01
#define LOG_LEVEL_ERROR 0x02
#define LOG_LEVEL_OFF 0x03

#define LOG_LEVEL_NAME_DEBUG "debug"
#define LOG_LEVEL_NAME_INFO "info"
#define LOG_LEVEL_NAME_ERROR "error"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#define LOGGER_CALL(tag, ...) _logIt(SourceLoc{FMT_FILE, __LINE__, LOGGER_FUNCTION}, tag, __VA_ARGS__)

#define LOG_FMT "[%s] [%s:%d] "

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) LOGGER_CALL(LOG_LEVEL_NAME_DEBUG, __VA_ARGS__)
#else
#define LOG_DEBUG(...) void(0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) LOGGER_CALL(LOG_LEVEL_NAME_INFO, __VA_ARGS__)
#else
#define LOG_INFO(...) void(0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(...) LOGGER_CALL(LOG_LEVEL_NAME_ERROR, __VA_ARGS__)
#else
#define LOG_ERROR(...) void(0)
#endif

struct SourceLoc {
    constexpr SourceLoc() = default;
    constexpr SourceLoc(const char* filenameIn, int lineIn, const char* funcnameIn) :
        filename{filenameIn},
        line{lineIn},
        funcname{funcnameIn}
    {}

    constexpr bool empty() const noexcept { return line == 0; }
    const char* filename{nullptr};
    int line{0};
    const char* funcname{nullptr};
};

inline void _logIt(const SourceLoc loc, const char* tag, const char* fmt, ...) noexcept
{
    static char buffer[256];
    sprintf(buffer, LOG_FMT, tag, loc.filename, loc.line);
    va_list args;
    va_start(args, fmt);
    vsprintf(&buffer[strlen(buffer)], fmt, args);
    va_end(args);
    Serial.println(buffer);
}
