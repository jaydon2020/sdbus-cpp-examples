// Copyright (c) 2026 Joel Winarske
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_UTILS_LOGGING_H
#define SRC_UTILS_LOGGING_H

/**
 * @brief Centralized logging header - SINGLE PLACE spdlog.h is included
 *
 * This header consolidates all logging infrastructure:
 * - spdlog.h (the only place it's included from)
 * - Logging configuration (environment-based setup)
 * - Error reporting macros (standardized logging levels)
 *
 * All other files should include this header instead of including spdlog
 * directly.
 *
 * Usage:
 *   #include "src/utils/logging.h"
 *
 *   LOG_INFO("Message");
 *   LOG_DEBUG("Debug info");
 *   LOG_ERROR("Error occurred");
 */

// ============================================================================
// SINGLE INCLUDE OF SPDLOG - ONLY LOCATION IN CODEBASE
// ============================================================================
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// ============================================================================
// LOGGING CONFIGURATION - Environment-based setup
// ============================================================================

namespace logging_config {

/**
 * @brief Get log level from the environment variable or default to info
 * @return spdlog::level::level_enum
 */
inline spdlog::level::level_enum getLogLevelFromEnv() {
  const char* level_str = std::getenv("LOG_LEVEL");
  if (!level_str) {
    return spdlog::level::info;
  }

  const std::string level{level_str};
  if (level == "trace")
    return spdlog::level::trace;
  if (level == "debug")
    return spdlog::level::debug;
  if (level == "info")
    return spdlog::level::info;
  if (level == "warn")
    return spdlog::level::warn;
  if (level == "err")
    return spdlog::level::err;
  if (level == "critical")
    return spdlog::level::critical;
  if (level == "off")
    return spdlog::level::off;

  return spdlog::level::info;
}

/**
 * @brief Check if file logging is enabled via an environment variable
 * @return std::string path to a log file, empty string if disabled
 */
inline std::string getLogFilePathFromEnv() {
  const char* log_file = std::getenv("LOG_FILE");
  return log_file ? std::string(log_file) : "";
}

/**
 * @brief Initialize logging with console and optional file output
 * @param logger_name Name of the logger instance
 */
inline void initializeLogging(const std::string& logger_name = "default") {
  try {
    const auto level = getLogLevelFromEnv();
    auto log_file = getLogFilePathFromEnv();
    const char* pattern_str = std::getenv("LOG_PATTERN");
    const std::string pattern = pattern_str ? std::string(pattern_str)
                                            : "[%Y-%m-%d %H:%M:%S.%e] [%l] %v";

    const auto console_sink =
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(level);

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_sink);

    if (!log_file.empty()) {
      const auto file_sink =
          std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
              log_file, 10 * 1024 * 1024, 3);
      file_sink->set_level(level);
      sinks.push_back(file_sink);
    }

    const auto logger = std::make_shared<spdlog::logger>(
        logger_name, sinks.begin(), sinks.end());
    logger->set_level(level);
    logger->set_pattern(pattern);
    logger->flush_on(spdlog::level::err);

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::info("Logging initialized - Level: {}, File: {}",
                 spdlog::level::to_string_view(level),
                 log_file.empty() ? "console only" : log_file);
  } catch (const spdlog::spdlog_ex& ex) {
    spdlog::error("Log initialization failed: {}", ex.what());
  }
}

// Helpers for callers that need runtime level/flush configuration without
// referencing spdlog symbols directly.
inline void setLevelDebug() {
  spdlog::set_level(spdlog::level::debug);
}
inline void setLevelInfo() {
  spdlog::set_level(spdlog::level::info);
}
inline void setFlushInterval(const std::chrono::seconds interval) {
  spdlog::flush_every(interval);
}

}  // namespace logging_config

// ============================================================================
// ERROR REPORTING MACROS - Standardized logging levels
// ============================================================================

/**
 * @brief TRACE - Development only: detailed traces, function entry/exit
 */
#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)

/**
 * @brief DEBUG - Development/troubleshooting: state changes, discoveries
 */
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)

/**
 * @brief INFO - Normal operation: startup/shutdown, significant events
 * (DEFAULT)
 */
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)

/**
 * @brief WARN - Recoverable errors: retries, degradation, resource warnings
 */
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)

/**
 * @brief ERROR - Critical failures: hard limits, invariant violations
 */
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

/**
 * @brief CRITICAL - Fatal conditions: data corruption, immediate shutdown
 */
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

// ============================================================================
// ENVIRONMENT VARIABLES FOR LOGGING CONFIGURATION
// ============================================================================
//
// LOG_LEVEL={trace,debug,info,warn,err,critical,off} - Set log level (default:
// info) LOG_FILE=/path/to/file - Enable file logging with automatic rotation
// LOG_PATTERN="[%t] [%l] %v" - Custom spdlog pattern string
//
// Examples:
//   LOG_LEVEL=debug ./app
//   LOG_FILE=/var/log/app.log LOG_LEVEL=info ./app
//   LOG_PATTERN="[%H:%M:%S] [%l] %v" LOG_LEVEL=debug ./app

// ============================================================================
// ERROR REPORTING GUIDELINES - Standardized logging severity reference
// ============================================================================
//
// LOG_TRACE  - Development only: detailed traces, function entry/exit
// LOG_DEBUG  - Development/troubleshooting: state changes, discoveries
// LOG_INFO   - Normal operation: startup/shutdown, significant events (DEFAULT)
// LOG_WARN   - Recoverable errors: retries, degradation, resource warnings
// LOG_ERROR  - Fatal errors: critical failures, hard limits, invariants broken
// LOG_CRITICAL - Application fatal: data corruption, immediate shutdown
//
// Decision tree for choosing level:
//   Is this a debug/internal detail? -> LOG_DEBUG
//   Is this a normal operation milestone? -> LOG_INFO
//   Is the error recoverable/expected to retry? -> LOG_WARN
//   Can the operation continue? NO -> LOG_ERROR, YES -> LOG_WARN
//   Is this application-fatal? -> LOG_CRITICAL before exit

#endif  // SRC_UTILS_LOGGING_H
