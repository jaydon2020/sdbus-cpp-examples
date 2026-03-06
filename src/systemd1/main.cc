#include <chrono>

#include "systemd1_manager_client.h"
#include "../utils/signal_handler.h"

int main() {
  try {
    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    Systemd1ManagerClient manager(*connection);

    LOG_INFO("Systemd1 monitor daemon running - Press Ctrl+C to exit");

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