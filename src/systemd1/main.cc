#include <chrono>
#include <thread>
#include "systemd1_manager_client.h"
#include "../utils/signal_handler.h"

int main() {
  installSignalHandlers();

  const auto connection = sdbus::createSystemBusConnection();
  connection->enterEventLoopAsync();

  Systemd1ManagerClient client(*connection);

  // Optional unit start (avoid direct Unit_proxy instantiation; use generic
  // proxy)
  if (const char* unitPathEnv = std::getenv("SYSTEMD_UNIT_PATH")) {
    try {
      const auto unitPath = sdbus::ObjectPath(unitPathEnv);
      const auto unitProxy = sdbus::createProxy(
          *connection, sdbus::ServiceName(Systemd1ManagerClient::SERVICE_NAME),
          unitPath);
      unitProxy->callMethod("Start")
          .onInterface(org::freedesktop::systemd1::Unit_proxy::INTERFACE_NAME)
          .withArguments(std::string("replace"));
      spdlog::info("Attempted Start on unit {}", unitPathEnv);
    } catch (const sdbus::Error& e) {
      spdlog::error("Failed to start unit {}: {} - {}", unitPathEnv,
                    e.getName(), e.getMessage());
    }
  }

  using namespace std::chrono_literals;
  spdlog::info("Systemd1 client running - Press Ctrl+C to exit");

  while (g_running) {
    std::this_thread::sleep_for(100ms);
  }

  spdlog::info("Shutting down...");
  connection->leaveEventLoop();
  return 0;
}