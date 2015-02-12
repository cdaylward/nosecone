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

#include <memory>


namespace nosecone {
namespace executor {


class AbstractContainer {
public:
  virtual ~AbstractContainer() {}
  virtual Status create_rootfs() = 0;
  virtual Status create_pty() = 0;
  virtual Status start() = 0;
  virtual pid_t clone_pid() const = 0;
  virtual int console_fd() const = 0;
};


class Container : public AbstractContainer {
private:
  const std::shared_ptr<AbstractContainer> impl;
public:
  Container(AbstractContainer* impl)
  : impl(impl) {}
  virtual ~Container() {}
  virtual Status create_rootfs() {
    return impl->create_rootfs();
  }
  virtual Status create_pty() {
    return impl->create_pty();
  }
  virtual Status start() {
    return impl->start();
  }
  virtual pid_t clone_pid() const {
    return impl->clone_pid();
  }
  virtual int console_fd() const {
    return impl->console_fd();
  }
};


} // namespace executor
} // namespace nosecone
