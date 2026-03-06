#include "network1_client.h"
#include "../utils/signal_handler.h"

#include <chrono>

int main() {
  try {
    installSignalHandlers();

    const auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    Network1ManagerClient client(*connection);

    using namespace std::chrono_literals;
    spdlog::info("Network1 client running - Press Ctrl+C to exit");

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