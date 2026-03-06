#include "../utils/signal_handler.h"
#include "network1_client.h"

#include <chrono>

int main() {
  try {
    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    Network1ManagerClient network_manager(*connection);

    LOG_INFO("network1 monitor daemon running - Press Ctrl+C to exit");

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