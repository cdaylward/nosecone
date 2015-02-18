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

#include <string>
#include <fstream>
#include <iostream>

#include "3rdparty/cdaylward/pathname.h"
#include "3rdparty/nlohmann/json.h"
#include "nosecone/executor/list.h"


namespace nosecone {
namespace executor {


using Json = nlohmann::json;

static Json container_info(const std::string& container_root) {
  const auto info_file = pathname::join(container_root, "info");
  Json json{};
  json["created"] = "N/A";
  json["has_pty"] = "N/A";
  json["id"] = "N/A";
  json["pid"] = "N/A";
  std::ifstream info(info_file);
  if (info) {
    info >> json;
    info.close();
  }
  return json;
}

int list(const std::string& container_dir) {
  // This is just a stub.
  auto dir = opendir(container_dir.c_str());
  if (dir == NULL) return EXIT_FAILURE;
  if (isatty(STDOUT_FILENO)) {
    std::cerr << "Container ID" << std::endl;
  }
  for (auto entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
    const std::string filename{entry->d_name};
    if (filename == "." || filename == "..") continue;
    const std::string full_path = pathname::join(container_dir, filename);
    const auto info = container_info(full_path);
    std::cout << filename << " " << info["created"] << " " << info["pid"];
    std::cout << " " << info["has_pty"] << std::endl;
  }
  closedir(dir);

  return EXIT_SUCCESS;
}


} // namespace executor
} // namespace nosecone
