#include <chrono>

#include "systemd1_manager_client.h"
#include "../utils/signal_handler.h"

int main() {
  try {
    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    Systemd1ManagerClient manager(*connection);

    spdlog::info("Systemd1 monitor daemon running - Press Ctrl+C to exit");

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