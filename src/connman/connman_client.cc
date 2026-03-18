// Copyright (c) 2026 Jian De
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

#include "connman_client.h"

#include <sstream>

#include "../utils/logging.h"
#include "../utils/utils.h"

//
// ConnmanManagerClient
//

ConnmanManagerClient::ConnmanManagerClient(sdbus::IConnection& connection)
    : ProxyInterfaces(connection,
                      sdbus::ServiceName{SERVICE_NAME},
                      sdbus::ObjectPath{OBJECT_PATH}) {
  registerProxy();

  // Load initial technologies
  for (const auto& tech : GetTechnologies()) {
    onTechnologyAdded(tech.get<0>(), tech.get<1>());
  }

  // Load initial services
  for (const auto& service : GetServices()) {
    const auto& path = service.get<0>();
    if (services_.find(path) == services_.end()) {
      services_[path] = std::make_unique<ConnmanServiceClient>(
          getProxy().getConnection(), path);
    }
  }
}

ConnmanManagerClient::~ConnmanManagerClient() {
  unregisterProxy();
}

void ConnmanManagerClient::onPropertyChanged(const std::string& name,
                                             const sdbus::Variant& value) {
  std::ostringstream os;
  Utils::append_property(value, os);
  LOG_INFO("Manager PropertyChanged: {} = {}", name, os.str());
}

void ConnmanManagerClient::onTechnologyAdded(
    const sdbus::ObjectPath& path,
    const std::map<std::string, sdbus::Variant>& properties) {
  LOG_INFO("Technology added: {}", path);
  if (technologies_.find(path) == technologies_.end()) {
    technologies_[path] = std::make_unique<ConnmanTechnologyClient>(
        getProxy().getConnection(), path);
  }
}

void ConnmanManagerClient::onTechnologyRemoved(const sdbus::ObjectPath& path) {
  LOG_INFO("Technology removed: {}", path);
  technologies_.erase(path);
}

void ConnmanManagerClient::onServicesChanged(
    const std::vector<sdbus::Struct<sdbus::ObjectPath,
                                    std::map<std::string, sdbus::Variant>>>&
        changed,
    const std::vector<sdbus::ObjectPath>& removed) {
  for (const auto& service : changed) {
    const auto& path = service.get<0>();
    const auto& props = service.get<1>();

    if (services_.find(path) == services_.end()) {
      LOG_INFO("Service added: {}", path);
      services_[path] = std::make_unique<ConnmanServiceClient>(
          getProxy().getConnection(), path);
    }

    // Log WiFi info only when Type is explicitly "wifi" in this delta
    bool is_wifi = false;
    if (auto it = props.find("Type");
        it != props.end() && it->second.containsValueOfType<std::string>()) {
      is_wifi = (it->second.get<std::string>() == "wifi");
    }

    if (is_wifi) {
      std::string ssid = "Unknown";
      if (auto it = props.find("Name");
          it != props.end() && it->second.containsValueOfType<std::string>()) {
        ssid = it->second.get<std::string>();
      }
      LOG_INFO("  WiFi Update: {} [{}]", ssid, path);
    }
  }

  for (const auto& path : removed) {
    LOG_INFO("  Service removed: {}", path);
    services_.erase(path);
  }
}

void ConnmanManagerClient::onPeersChanged(
    const std::vector<sdbus::Struct<sdbus::ObjectPath,
                                    std::map<std::string, sdbus::Variant>>>&,
    const std::vector<sdbus::ObjectPath>&) {}

void ConnmanManagerClient::onTetheringClientsChanged(
    const std::vector<std::string>&,
    const std::vector<std::string>&) {}

//
// ConnmanTechnologyClient
//

ConnmanTechnologyClient::ConnmanTechnologyClient(sdbus::IConnection& connection,
                                                 sdbus::ObjectPath path)
    : ProxyInterfaces(connection,
                      sdbus::ServiceName{SERVICE_NAME},
                      std::move(path)) {
  registerProxy();
}

ConnmanTechnologyClient::~ConnmanTechnologyClient() {
  unregisterProxy();
}

void ConnmanTechnologyClient::onPropertyChanged(const std::string& name,
                                                const sdbus::Variant& value) {
  if (name == "Scanning" && value.containsValueOfType<bool>()) {
    bool scanning = value.get<bool>();
    LOG_INFO("Technology [{}] WiFi Scan: {}", getProxy().getObjectPath(),
             scanning ? "STARTED" : "COMPLETED");
  }

  std::ostringstream os;
  Utils::append_property(value, os);
  LOG_INFO("Technology [{}] PropertyChanged: {} = {}",
           getProxy().getObjectPath(), name, os.str());
}

//
// ConnmanServiceClient
//

ConnmanServiceClient::ConnmanServiceClient(sdbus::IConnection& connection,
                                           sdbus::ObjectPath path)
    : ProxyInterfaces(connection,
                      sdbus::ServiceName{SERVICE_NAME},
                      std::move(path)) {
  registerProxy();
}

ConnmanServiceClient::~ConnmanServiceClient() {
  unregisterProxy();
}

void ConnmanServiceClient::onPropertyChanged(const std::string& name,
                                             const sdbus::Variant& value) {
  std::ostringstream os;
  Utils::append_property(value, os);
  LOG_INFO("Service [{}] PropertyChanged: {} = {}", getProxy().getObjectPath(),
           name, os.str());
}
