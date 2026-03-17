#pragma once

#include <spdlog/spdlog.h>

/// @file Log.hpp
/// @brief Thin wrappers around spdlog macros for application-wide logging.
///
/// Use these macros instead of calling spdlog directly so that the logging
/// backend can be swapped without touching call sites. The logger itself is
/// initialized in App::setupLogger().
///
/// All macros accept a printf-style format string followed by optional
/// arguments, forwarded directly to the corresponding SPDLOG_* macro.
///
/// @see App::setupLogger()

/// @brief Log a TRACE-level message (finest granularity, disabled in release).
#define QN_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)

/// @brief Log a DEBUG-level message.
#define QN_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)

/// @brief Log an INFO-level message.
#define QN_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)

/// @brief Log a WARNING-level message.
#define QN_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)

/// @brief Log an ERROR-level message.
#define QN_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
