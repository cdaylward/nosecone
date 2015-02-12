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

#pragma once

#include "3rdparty/cdaylward/pathname.h"
#include "nosecone/executor/container.h"
#include "nosecone/executor/image.h"


namespace nosecone {
namespace executor {
namespace container {
namespace linux {


class Container : public nosecone::executor::Container {
private:
  class Impl : public nosecone::executor::AbstractContainer {
  private:
    const std::string uuid;
    const std::string container_root_path;
    const std::string rootfs_path;
    std::vector<Image> images;
    int console_master_fd;
    std::string console_slave_name;
    pid_t pid;
  public:
    Impl(const std::string& uuid,
         const std::string& container_root_path,
         std::vector<Image>& images)
    : uuid(uuid),
      container_root_path(container_root_path),
      rootfs_path(pathname::join(container_root_path, "rootfs")),
      images(images),
      console_master_fd(-1),
      console_slave_name(""),
      pid(-1) {}
    virtual Status create_rootfs();
    virtual Status create_pty();
    virtual Status start();
    virtual pid_t clone_pid() const;
    virtual int console_fd() const;
  };
public:
  Container(const std::string& uuid,
            const std::string& container_root_path,
            std::vector<Image>& images)
  : nosecone::executor::Container(new Impl(uuid, container_root_path, images)) {}
};

bool parent_of(const Container& container);
Status await(const Container& container);

} // namespace linux
} // namespace container
} // namespace executor
} // namespace nosecone
