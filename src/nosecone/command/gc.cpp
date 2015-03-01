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

#include <iostream>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "3rdparty/cdaylward/pathname.h"

#include "appc/os/errno.h"
#include "appc/util/try.h"
#include "appc/util/status.h"

#include "nosecone/command.h"
#include "nosecone/config.h"
#include "nosecone/executor/status.h"

#include "nosecone/command/gc.h"


namespace nosecone {
namespace command {


static Status recursive_remove_dir(const std::string& path) {
  struct stat st;

  if (lstat(path.c_str(), &st) < 0) {
    return Errno(std::string{"Couldn't stat "} + path, errno);
  }

  if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
    if (unlink(path.c_str()) < 0) {
      return Errno(std::string{"Could not remove "} + path, errno);
    }
    return Success();
  } else if (S_ISDIR(st.st_mode)) {
    auto dir = opendir(path.c_str());
    if (dir == NULL) {
      return Errno(std::string{"Could not traverse "} + path, errno);
    }
    for (auto entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
      const std::string filename{entry->d_name};
      if (filename == "." || filename == "..") continue;
      const std::string next_path{pathname::join(path, filename)};
      auto rmd = recursive_remove_dir(next_path);
      if (!rmd) {
        closedir(dir);
        return rmd;
      }
    }
    closedir(dir);
    if (rmdir(path.c_str()) < 0) {
      return Errno(std::string{"Could not remove "} + path, errno);
    }
    return Success();
  }

  return Error(std::string{"Could not remove "} + path);
}


int perform_gc(const Arguments& args) {
  auto dir = opendir(config.containers_path.c_str());
  if (dir == NULL) {
    std::cerr << "Could not open " << config.containers_path << ": " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  for (auto entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
    const std::string filename{entry->d_name};
    if (filename == "." || filename == "..") continue;
    const auto status_try = executor::container_status(filename);
    if (!status_try) {
      std::cerr << filename << " does not appear to be an app container." << std::endl;
      continue;
    }
    const auto status = from_result(status_try);
    if (status.running) continue;
    // TODO these are sprinkled around. Make function, make safe.
    const std::string full_path{pathname::join(config.containers_path, filename)};
    auto gced = recursive_remove_dir(full_path);
    if (!gced) {
      std::cerr << gced.message << std::endl;
    }
    std::cout << filename << " expunged." << std::endl;
  }

  return EXIT_SUCCESS;
}


} // namespace command
} // namespace nosecone
