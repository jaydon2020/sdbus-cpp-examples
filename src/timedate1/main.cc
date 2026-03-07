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

#include "timedate1_client.h"

#include <array>

#include "../utils/logging.h"
#include "../utils/utils.h"

int main() {
  const auto connection = sdbus::createSystemBusConnection();
  connection->enterEventLoopAsync();

  Timedate1Client client(*connection);

  std::array<std::future<std::map<sdbus::PropertyName, sdbus::Variant>>, 4>
      futures;
  std::array<std::promise<std::map<sdbus::PropertyName, sdbus::Variant>>, 4>
      promises;

  for (int i = 0; i < 4; ++i) {
    futures.at(i) = promises.at(i).get_future();
    client.GetAllAsync(
        Timedate1Client::INTERFACE_NAME,
        [&, i](std::optional<sdbus::Error> error,
               std::map<sdbus::PropertyName, sdbus::Variant> values) {
          if (!error)
            promises.at(i).set_value(std::move(values));
          else
            promises.at(i).set_exception(std::make_exception_ptr(*error));
        });
  }

  for (auto& future : futures) {
    try {
      const auto properties = future.get();
      client.updateTimedate1(properties);
      client.printTimedate1();
    } catch (const std::exception& e) {
      LOG_ERROR("Error: {}", e.what());
    }
  }

  connection->leaveEventLoop();

  return 0;
}
