#include "wpa_supplicant1_client.h"
#include "../utils/signal_handler.h"

#include <chrono>

int main() {
  installSignalHandlers();

  const auto connection = sdbus::createSystemBusConnection();
  connection->enterEventLoopAsync();

  WpaSupplicant1Client client(*connection);

  using namespace std::chrono_literals;
  spdlog::info("WPA Supplicant client running - Press Ctrl+C to exit");

  // Monitor loop with connection health checks every 30 seconds
  auto result = monitorLoop(*connection, 30s, 100ms);

  if (result) {
    spdlog::error("Exiting due to: {}", *result);
  } else {
    spdlog::info("Shutting down...");
  }

  connection->leaveEventLoop();
  return result ? 1 : 0;
}