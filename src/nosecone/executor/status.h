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

#include "3rdparty/nlohmann/json.h"


namespace nosecone {
namespace executor {


using Json = nlohmann::json;


struct ContainerStatus {
  const std::string id;
  const time_t created_time;
  const bool has_pty;
  const pid_t pid;
  const bool running;
};


Try<Json> container_info(const std::string& container_root);


Try<ContainerStatus> container_status(const std::string& dir);


} // namespace executor
} // namespace nosecone
