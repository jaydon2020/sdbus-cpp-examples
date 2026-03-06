// Copyright (c) 2025 Joel Winarske
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

#include <chrono>

#include "geoclue2_manager.h"
#include "../utils/signal_handler.h"

int main() {
  try {
    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    const GeoClue2Manager manager(
        *connection, [&](const GeoClue2Location& location) {
          const auto [Accuracy, Altitude, Description, Heading, Latitude,
                      Longitude, Speed, Timestamp] = location.Properties();
          spdlog::info("Timestamp: {}.{}", Timestamp.tv_sec, Timestamp.tv_nsec);
          spdlog::info("Lat/Long: {}, {}", Latitude, Longitude);
          spdlog::info("Heading: {}", Heading);
          spdlog::info("Speed: {}", Speed);
          spdlog::info("Accuracy: {}", Accuracy);
          spdlog::info("Altitude: {}", Altitude);
          spdlog::info("Description: {}", Description);
        });

    const auto& client = manager.Client();

    // `desktop id` must be set for Start to work
    client->DesktopId("org.example.geoclue2");
    client->Start();

    spdlog::info("Geoclue2 monitor daemon running - Press Ctrl+C to exit");

    auto result = monitorLoop(*connection);

    if (result) {
      spdlog::error("Exiting due to: {}", *result);
    } else {
      spdlog::info("Shutting down...");
    }

    connection->leaveEventLoop();
    return result ? 1 : 0;

  } catch (const sdbus::Error& e) {
    spdlog::error("D-Bus error: {} - {}", e.getName(), e.getMessage());
    return 1;
  } catch (const std::exception& e) {
    spdlog::error("Exception: {}", e.what());
    return 1;
  }
}
