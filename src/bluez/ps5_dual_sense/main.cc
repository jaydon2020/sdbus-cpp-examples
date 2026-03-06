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

#include "../../utils/signal_handler.h"
#include "dual_sense.h"

int main() {
  try {
    installSignalHandlers();

    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_every(kLogFlushInterval);

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    DualSense client(*connection);

    LOG_INFO("PS5 DualSense client running - Press Ctrl+C to exit");

    // Monitor loop with shared connection health timing defaults
    auto result = monitorLoop(*connection);

    if (result) {
      LOG_ERROR("Exiting due to: {}", *result);
    } else {
      LOG_INFO("Shutting down...");
    }

    connection->leaveEventLoop();
    return result ? 1 : 0;

  } catch (const sdbus::Error& e) {
    LOG_ERROR("D-Bus error: {} - {}", e.getName(), e.getMessage());
    return 1;
  } catch (const std::exception& e) {
    LOG_ERROR("Exception: {}", e.what());
    return 1;
  }
}
