
#include "packagekit_client.h"

#include "../utils/logging.h"
#include "../utils/utils.h"

PackageKitClient::PackageKitClient(sdbus::IConnection& connection)
    : ProxyInterfaces{connection, sdbus::ServiceName(INTERFACE_NAME),
                      sdbus::ObjectPath(OBJECT_PATH)},
      object_path_(sdbus::ObjectPath(OBJECT_PATH)) {
  registerProxy();
}

PackageKitClient::~PackageKitClient() {
  unregisterProxy();
}

void PackageKitClient::onTransactionListChanged(
    const std::vector<std::string>& transactions) {
  std::ostringstream os;
  os << std::endl;
  os << "[PackageKitClient] onTransactionListChanged (" << transactions.size()
     << ")" << std::endl;
  for (const auto& transaction : transactions) {
    os << transaction << std::endl;
  }
  LOG_INFO(os.str());
}

void PackageKitClient::onRestartSchedule() {
  LOG_INFO("onRestartSchedule");
}

void PackageKitClient::onRepoListChanged() {
  LOG_INFO("onRepoListChanged");
}

void PackageKitClient::onUpdatesChanged() {
  LOG_INFO("onUpdatesChanged");
}

void PackageKitClient::onPropertiesChanged(
    const sdbus::InterfaceName& interfaceName,
    const std::map<sdbus::PropertyName, sdbus::Variant>& changedProperties,
    const std::vector<sdbus::PropertyName>& invalidatedProperties) {
  Utils::print_changed_properties(interfaceName, changedProperties,
                                  invalidatedProperties);
}
