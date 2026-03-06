#include "wpa_supplicant1_client.h"
#include "../utils/signal_handler.h"

#include <chrono>
#include <thread>

int main() {
  installSignalHandlers();

  const auto connection = sdbus::createSystemBusConnection();
  connection->enterEventLoopAsync();

  WpaSupplicant1Client client(*connection);

  using namespace std::chrono_literals;
  spdlog::info("WPA Supplicant client running - Press Ctrl+C to exit");

  while (g_running) {
    std::this_thread::sleep_for(100ms);
  }

  spdlog::info("Shutting down...");
  connection->leaveEventLoop();
  return 0;
}