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

#ifndef SRC_UTILS_PROPERTY_UTILS_H
#define SRC_UTILS_PROPERTY_UTILS_H

#include <sdbus-c++/sdbus-c++.h>
#include <map>
#include <optional>
#include <string>
#include "../utils/logging.h"

namespace property_utils {

/**
 * @brief Safely get a D-Bus property value with type checking
 *
 * @tparam T The expected type of the property
 * @param properties The property map
 * @param key The property name to retrieve
 * @return std::optional<T> The property value if found and correct type,
 * nullopt otherwise
 */
template <typename T>
std::optional<T> getProperty(
    const std::map<sdbus::PropertyName, sdbus::Variant>& properties,
    const sdbus::PropertyName& key) {
  // Check if a property exists
  const auto it = properties.find(key);
  if (it == properties.end()) {
    LOG_DEBUG("Property '{}' not found", key);
    return std::nullopt;
  }

  // Try to get the value with type checking
  try {
    return it->second.get<T>();
  } catch (const sdbus::Error& e) {
    LOG_ERROR("D-Bus error getting property '{}': {} - {}", key, e.getName(),
              e.getMessage());
    return std::nullopt;
  } catch (const std::exception& e) {
    LOG_ERROR("Exception getting property '{}': {}", key, e.what());
    return std::nullopt;
  }
}

/**
 * @brief Safely get a D-Bus property value with type checking and default value
 *
 * @tparam T The expected type of the property
 * @param properties The property map
 * @param key The property name to retrieve
 * @param default_value The default value to return if property not found or
 * wrong type
 * @return T The property value if found and correct type, default_value
 * otherwise
 */
template <typename T>
T getPropertyOr(const std::map<sdbus::PropertyName, sdbus::Variant>& properties,
                const sdbus::PropertyName& key,
                const T& default_value) {
  auto result = getProperty<T>(properties, key);
  return result.value_or(default_value);
}

/**
 * @brief Check if a property exists in the map
 *
 * @param properties The property map
 * @param key The property name to check
 * @return bool True if property exists, false otherwise
 */
inline bool hasProperty(
    const std::map<sdbus::PropertyName, sdbus::Variant>& properties,
    const sdbus::PropertyName& key) {
  return properties.contains(key);
}

/**
 * @brief Safely get a required D-Bus property value
 *
 * Throws std::runtime_error if property not found or wrong type.
 * Use this for properties that are mandatory for correct operation.
 *
 * @tparam T The expected type of the property
 * @param properties The property map
 * @param key The property name to retrieve
 * @return T The property value
 * @throws std::runtime_error if property not found or wrong type
 */
template <typename T>
T getRequiredProperty(
    const std::map<sdbus::PropertyName, sdbus::Variant>& properties,
    const sdbus::PropertyName& key) {
  auto result = getProperty<T>(properties, key);
  if (!result) {
    throw std::runtime_error("Required property '" + std::string(key) +
                             "' not found or has wrong type");
  }
  return *result;
}

}  // namespace property_utils

#endif  // SRC_UTILS_PROPERTY_UTILS_H
