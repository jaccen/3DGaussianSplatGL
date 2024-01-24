#pragma once

#include <array>
#include <string>
#ifdef _MSC_VER
    #include <windows.h>
#endif

namespace jaccen
{
    enum class LogLevel
    {
        Info = 0,
        Error
    };

    template <typename... Args>
    void log(jaccen::LogLevel level, const char* format, Args... args)
    {
        std::FILE* output = (level == LogLevel::Info ? stdout : stderr);
        std::fprintf(output, format, args...);
        std::fprintf(output, "\n");

#ifdef _MSC_VER
        char buffer[512]; \
        sprintf_s(buffer, sizeof(buffer), format, args...); \
        OutputDebugStringA(buffer);
        OutputDebugStringA("\n");
#endif
    }
}

#define ECS_LOG(format, ...) \
        do { \
        jaccen::log(jaccen::LogLevel::Info, "[%s:%d] " format, __FILE__, __LINE__, ##__VA_ARGS__); } \
        while (false)

#define ECS_LOG_ERROR(format, ...) \
        do { \
        jaccen::log(jaccen::LogLevel::Error, "[%s:%d] ERROR: " format, __FILE__, __LINE__, ##__VA_ARGS__); } \
        while (false)
