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

#include <dirent.h>

#include <string>
#include <iostream>

#include "nosecone/config.h"
#include "nosecone/list.h"


extern nosecone::Config config;


namespace nosecone {


int list(const std::string& container_dir) {
  auto dir = opendir(container_dir.c_str());
  if (dir == NULL) return EXIT_FAILURE;
  for (auto entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
    const std::string filename{entry->d_name};
    if (filename == "." || filename == "..") continue;
    std::cout << filename << std::endl;
  }
  closedir(dir);

  return EXIT_SUCCESS;
}


int process_list_args(const std::vector<std::string>& args) {
  return list(config.containers_path);
}


} // namespace nosecone
