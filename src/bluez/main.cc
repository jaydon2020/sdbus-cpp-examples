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

#include "bluez_client.h"
#include "../utils/signal_handler.h"


int main() {
  try {
    // Initialize logging with environment variable configuration
    logging_config::initializeLogging("bluez_client");

    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    BluezClient client(*connection);

    LOG_INFO("BlueZ client running - Press Ctrl+C to exit");

    // Monitor loop with shared connection health timing defaults
    auto result = monitorLoop(*connection);

    if (result) {
      // Connection was lost
      LOG_ERROR("Exiting due to: {}", *result);
    } else {
      // Graceful shutdown via signal
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
