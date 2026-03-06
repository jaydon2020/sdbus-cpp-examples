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

#include <spdlog/spdlog.h>

#include "dual_sense.h"
#include "../../utils/signal_handler.h"

int main() {
  try {
    installSignalHandlers();

    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(5));

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    DualSense client(*connection);

    using namespace std::chrono_literals;
    spdlog::info("PS5 DualSense client running - Press Ctrl+C to exit");

    // Monitor loop with connection health checks every 30 seconds
    auto result = monitorLoop(*connection, 30s, 100ms);

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
