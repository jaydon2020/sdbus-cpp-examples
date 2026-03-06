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

#include "../utils/logging.h"
#include "realtimekit1_manager_client.h"

#include <sdbus-c++/sdbus-c++.h>

extern void exerciseRealtime(RealtimeKit1ManagerClient&);

int main() {
  try {
    const auto connection = sdbus::createSystemBusConnection();
    const auto proxy = sdbus::createProxy(
        *connection, sdbus::ServiceName("org.freedesktop.RealtimeKit1"),
        sdbus::ObjectPath("/org/freedesktop/RealtimeKit1"));
    RealtimeKit1ManagerClient client(*proxy);
    exerciseRealtime(client);
    LOG_INFO("RealtimeKit1 example complete");
  } catch (const sdbus::Error& e) {
    LOG_ERROR("D-Bus error: {} - {}", e.getName(), e.getMessage());
    return 1;
  } catch (const std::exception& e) {
    LOG_ERROR("Exception: {}", e.what());
    return 1;
  }
  return 0;
}
