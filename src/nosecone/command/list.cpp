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

#include <unistd.h>
#include <dirent.h>
#include <time.h>

#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <ctime>

#include "3rdparty/cdaylward/pathname.h"
#include "3rdparty/nlohmann/json.h"

#include "appc/util/try.h"

#include "nosecone/config.h"
#include "nosecone/command/list.h"


extern nosecone::Config config;


namespace nosecone {
namespace command {


using Json = nlohmann::json;

struct ContainerStatus {
  const std::string id;
  const time_t created_time;
  const bool has_pty;
  const pid_t pid;
  const bool running;
};

static Json container_info(const std::string& container_root) {
  const auto info_file = pathname::join(container_root, "info");
  Json json{};
  std::ifstream info(info_file);
  if (info) {
    info >> json;
    info.close();
  }
  return json;
}

static Try<ContainerStatus> container_status(const Json& info) {
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
  const bool running = stat(cmdline_filename.c_str(), &proc_stat) == 0;

  return Result(ContainerStatus{id, created_time, has_pty, pid, running});
}


int perform_list(const Arguments& args) {
  // TODO stub, clean up.
  auto dir = opendir(config.containers_path.c_str());
  if (dir == NULL) return EXIT_FAILURE;
  // TODO probably passe
  if (isatty(STDOUT_FILENO)) {
    std::cerr << std::left;
    std::cerr << std::setw(39) << "Container ID";
    std::cerr << std::setw(24) << "Created Date";
    std::cerr << std::setw(8) << "PID";
    std::cerr << std::setw(8) << "Has PTY";
    std::cerr << std::setw(6) << "Status" << std::endl;
  }
  for (auto entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
    const std::string filename{entry->d_name};
    if (filename == "." || filename == "..") continue;
    const std::string full_path = pathname::join(config.containers_path, filename);
    const auto info = container_info(full_path);
    const auto status = container_status(info);

    std::cout << std::left;
    std::cout << std::setw(39) << filename;

    std::cout << std::setw(24);
    if (status) {
      const auto created_time = from_result(status).created_time;
      char time[100];
      strftime(time, 99, "%Y-%m-%dT%H:%M:%S.0Z", gmtime(&created_time));
      std::cout << time;
    }
    else {
      std::cout << "N/A";
    }

    std::cout << std::right << std::setw(7);
    if (status) {
      const auto pid = from_result(status).pid;
      std::cout << pid;
    } else {
      std::cout << "N/A";
    }
    std::cout << " " << std::setw(7);

    if (status) {
      const auto has_pty = from_result(status).has_pty;
      std::cout << std::boolalpha << has_pty;
    } else {
      std::cout << "N/A";
    }

    std::cout << " ";
    if (status) {
      const auto running = from_result(status).running;
      std::cout << (running ? "RUNNING" : "EXITED");
    } else {
      std::cout << "N/A";
    }

    std::cout << std::endl;
  }
  closedir(dir);

  return EXIT_SUCCESS;
}


} // namespace command
} // namespace nosecone
