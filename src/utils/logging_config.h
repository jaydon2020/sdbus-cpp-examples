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

#ifndef SRC_UTILS_LOGGING_CONFIG_H
#define SRC_UTILS_LOGGING_CONFIG_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <cstdlib>
#include <string>
#include <memory>

namespace logging_config {

/**
 * @brief Get log level from environment variable or default to info
 *
 * Supported values: trace, debug, info, warn, err, critical, off
 * Default: info
 *
 * @return spdlog::level::level_enum
 */
inline spdlog::level::level_enum getLogLevelFromEnv() {
  const char* level_str = std::getenv("LOG_LEVEL");
  if (!level_str) {
    return spdlog::level::info;  // Default level
  }

  const std::string level{level_str};
  if (level == "trace") return spdlog::level::trace;
  if (level == "debug") return spdlog::level::debug;
  if (level == "info") return spdlog::level::info;
  if (level == "warn") return spdlog::level::warn;
  if (level == "err") return spdlog::level::err;
  if (level == "critical") return spdlog::level::critical;
  if (level == "off") return spdlog::level::off;

  // Default if invalid
  return spdlog::level::info;
}

/**
 * @brief Check if file logging is enabled via environment variable
 *
 * Set LOG_FILE=/path/to/logfile to enable file logging
 *
 * @return std::string path to a log file, empty string if disabled
 */
inline std::string getLogFilePathFromEnv() {
  const char* log_file = std::getenv("LOG_FILE");
  return log_file ? std::string(log_file) : "";
}

/**
 * @brief Initialize logging with console and optional file output
 *
 * Reads configuration from environment variables:
 * - LOG_LEVEL: Logging level (trace, debug, info, warn, err, critical, off)
 * - LOG_FILE: Path to a log file for file output (optional)
 * - LOG_PATTERN: Custom log pattern (optional)
 *
 * Default pattern: "[%Y-%m-%d %H:%M:%S.%e] [%l] %v"
 *
 * @param logger_name Name of the logger instance
 */
inline void initializeLogging(const std::string& logger_name = "default") {
  try {
    // Get configuration from the environment
    const auto level = getLogLevelFromEnv();
    auto log_file = getLogFilePathFromEnv();
    const char* pattern_str = std::getenv("LOG_PATTERN");
    const std::string pattern = pattern_str ? std::string(pattern_str)
                                      : "[%Y-%m-%d %H:%M:%S.%e] [%l] %v";

    // Create console sink
    const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(level);

    // Create a sink collection (use vector to avoid lifetime issues)
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_sink);

    if (!log_file.empty()) {
      // Create a rotating file sink (max 10MB, 3 rotations)
      const auto file_sink =
          std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
              log_file, 10 * 1024 * 1024, 3);
      file_sink->set_level(level);
      sinks.push_back(file_sink);
    }

    // Create a logger with a sink vector
    const auto logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
    logger->set_level(level);
    logger->set_pattern(pattern);
    logger->flush_on(spdlog::level::err);

    // Register as default logger
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::info("Logging initialized - Level: {}, File: {}",
                 spdlog::level::to_string_view(level),
                 log_file.empty() ? "console only" : log_file);
  } catch (const spdlog::spdlog_ex& ex) {
    spdlog::error("Log initialization failed: {}", ex.what());
  }
}

}  // namespace logging_config

#endif  // SRC_UTILS_LOGGING_CONFIG_H


