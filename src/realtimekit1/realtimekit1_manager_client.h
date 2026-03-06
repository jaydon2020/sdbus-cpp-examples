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

#ifndef SRC_REALTIMEKIT1_REALTIMEKIT1_MANAGER_CLIENT_H
#define SRC_REALTIMEKIT1_REALTIMEKIT1_MANAGER_CLIENT_H

#include "../proxy/org/freedesktop/RealtimeKit1/realtime_kit1_proxy.h"

#include <sdbus-c++/sdbus-c++.h>

#include <unistd.h>
#include <cstdint>

#include "../utils/logging.h"


class RealtimeKit1ManagerClient : public org::freedesktop::RealtimeKit1_proxy {
 public:
  explicit RealtimeKit1ManagerClient(sdbus::IProxy& proxy)
      : org::freedesktop::RealtimeKit1_proxy(proxy) {}

  void dumpProperties() {
    LOG_INFO("RTTimeUSecMax       : {}", RTTimeUSecMax());
    LOG_INFO("MaxRealtimePriority : {}", MaxRealtimePriority());
    LOG_INFO("MinNiceLevel        : {}", MinNiceLevel());
  }

  bool tryHighPriority(int32_t niceLevel) {
    try {
      auto tid = currentTid();
      LOG_INFO("MakeThreadHighPriority thread={} priority={}", tid,
                   niceLevel);
      MakeThreadHighPriority(tid, niceLevel);
      LOG_INFO("High priority change succeeded");
      return true;
    } catch (const sdbus::Error& e) {
      LOG_WARN("High priority change failed: {} ({})", e.getName(),
                   e.getMessage());
      return false;
    }
  }

  bool tryRealtime(uint32_t rtPriority) {
    try {
      auto tid = currentTid();
      LOG_INFO("MakeThreadRealtime thread={} rtPriority={}", tid,
                   rtPriority);
      MakeThreadRealtime(tid, rtPriority);
      LOG_INFO("Realtime change succeeded");
      return true;
    } catch (const sdbus::Error& e) {
      LOG_WARN("Realtime change failed: {} ({})", e.getName(),
                   e.getMessage());
      return false;
    }
  }

 private:
  static uint64_t currentTid() {
#ifdef SYS_gettid
    return static_cast<uint64_t>(::syscall(SYS_gettid));
#else
    return static_cast<uint64_t>(::getpid());
#endif
  }
};

#endif  // SRC_REALTIMEKIT1_REALTIMEKIT1_MANAGER_CLIENT_H
