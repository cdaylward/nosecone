// Copyright 2015 Charles D. Aylward
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// A (possibly updated) copy of of this software is available at
// https://github.com/cdaylward/nosecone

#include <fstream>

#include "3rdparty/cdaylward/pathname.h"
#include "nosecone/config.h"
#include "nosecone/executor/status.h"


extern nosecone::Config config;


namespace nosecone {
namespace executor {


using Json = nlohmann::json;


Try<Json> container_info(const std::string& container_root) {
  const auto info_file = pathname::join(container_root, "info");
  Json json{};
  std::ifstream info(info_file);
  if (!info) return Failure<Json>("Could not read container info JSON.");
  info >> json;
  info.close();
  return Result(json);
}


Try<ContainerStatus> container_status(const std::string& dir) {
  const std::string full_path = pathname::join(config.containers_path, dir);
  const auto info_try = container_info(full_path);
  if (!info_try) return Failure<ContainerStatus>(info_try.failure_reason());
  const auto info = from_result(info_try);
  if (info.find("id") == info.end() ||
      info.find("created") == info.end() ||
      info.find("has_pty") == info.end() ||
      info.find("pid") == info.end()) {
    return Failure<ContainerStatus>("Container info JSON missing fields.");
  }
  const std::string id = info["id"];
  const time_t created_time = info["created"];
  const bool has_pty = info["has_pty"];
  pid_t pid = info["pid"];
  struct stat proc_stat;
  const auto cmdline_filename = pathname::join("/proc", std::to_string(pid), "cmdline");
  // TODO Need another key besides PID existence to limit collisions.
  // Possibly store/check namespace keys.
  const bool running = stat(cmdline_filename.c_str(), &proc_stat) == 0;

  return Result(ContainerStatus{id, created_time, has_pty, pid, running});
}


} // namespace executor
} // namespace nosecone
