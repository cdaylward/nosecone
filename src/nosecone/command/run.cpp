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

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "3rdparty/cdaylward/pathname.h"
#include "3rdparty/nlohmann/json.h"

#include "appc/schema/image.h"
#include "appc/os/mkdir.h"

#include "nosecone/help.h"
#include "nosecone/config.h"
#include "nosecone/command/run.h"
#include "nosecone/executor/fetch.h"
#include "nosecone/executor/uuid.h"
#include "nosecone/executor/container.h"
#include "nosecone/executor/container/linux.h"

#include "appc/discovery/types.h"


namespace nosecone {
namespace command {


using Json = nlohmann::json;
using namespace nosecone::executor;


static Json to_json(const Container& container) {
  Json json{};
  // TODO Move to container start time, not to_json time.
  using clock = std::chrono::system_clock;
  json["created"] = std::chrono::duration_cast<std::chrono::seconds>(
      clock::now().time_since_epoch()).count();
  json["id"] = container.id();
  json["pid"] = container.pid();
  json["has_pty"] = container.has_pty();
  return json;
}


static void dump_container_stdout(const Container& container) {
  const int pty_master_fd = container.pty_fd();
  char pty_buffer[4096];
  for (ssize_t rc = 0;
       rc != -1;
       rc = read(pty_master_fd, pty_buffer, sizeof(pty_buffer) - 1)) {
    if (rc > 0) {
      pty_buffer[rc] = '\0';
      std::cout << pty_buffer;
    }
  }
}


int perform_run(const Arguments& args) {
  if (args.ordered_args.size() < 1) {
    std::cerr << "Missing argument: <app name>" << std::endl << std::endl;
    print_help(command::run);
    return EXIT_FAILURE;
  }

  const appc::discovery::Name name{args.ordered_args[0]};

  appc::discovery::Labels labels = config.default_labels;

  if (args.ordered_args.size() > 1) {
    for (auto iter = args.ordered_args.begin() + 1; iter != args.ordered_args.end(); iter++) {
      auto& label_set = *iter;
      auto delim = label_set.find(":");
      if (delim == std::string::npos ||
          delim == label_set.length() - 1) {
        std::cerr << "Additional argument that isn't a label: " << label_set << std::endl;
        return EXIT_FAILURE;
      }
      auto name = label_set.substr(0, delim);
      auto value = label_set.substr(delim + 1);
      labels[name] = value;
    }
  }

  const bool wait_for_container = args.has_flag("wait") ? true : false;
  const bool dump_stdout = args.has_flag("stdout") ? true : false;

  if (geteuid() != 0) {
    std::cerr << "Containers can only be started by root." << std::endl;
    return EXIT_FAILURE;
  }

  const auto made_nosecone_root = appc::os::mkdir(config.containers_path, 0755, true);
  if (!made_nosecone_root) {
    std::cerr << "Could not create dir for containers: " << made_nosecone_root.message << std::endl;
    return EXIT_FAILURE;
  }

  auto images_try = fetch_and_validate(name, labels, true);
  if (!images_try) {
    std::cerr << images_try.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }

  auto images = from_result(images_try);

  if (!images[0].manifest.app) {
    std::cerr << "Image has no app, nothing to run." << std::endl;
    return EXIT_FAILURE;
  }

  const auto uuid_try = new_uuid();
  if (!uuid_try) {
    std::cerr << "Could not generate uuid: " << uuid_try.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }

  const auto uuid = from_result(uuid_try);

  std::cerr << "Container ID: " << uuid << std::endl;

  const std::string container_root = pathname::join(config.containers_path, uuid);
  auto container = container::linux::Container(uuid, container_root, images);

  container.create_rootfs();
  // always?
  container.create_pty();
  auto started = container.start();
  if (!started) {
    std::cerr << "Could not start container: " << started.message << std::endl;
    return EXIT_FAILURE;
  }

  // REM we are potentially cloned (forked) here, so check that we are the parent of the
  // container before doing parent things.
  if (parent_of(container)) {
    std::cerr << "Container started, PID: " << container.pid() << std::endl;
    // TODO move this to container.start...probably.
    auto info_json = to_json(container);
    const auto container_info_file = pathname::join(container_root, "info");
    std::ofstream info(container_info_file);
    if (info) {
      info << info_json.dump(4) << std::endl;
      info.close();
    }
    if (dump_stdout) {
      std::cerr << "--- 8< ---" << std::endl;
      dump_container_stdout(container);
    } else if (wait_for_container) {
      auto waited = await(container);
    }
  }

  return EXIT_SUCCESS;
}


} // namespace command
} // namespace nosecone
