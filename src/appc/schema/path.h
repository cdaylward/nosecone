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
// https://github.com/cdaylward/libappc

#pragma once

#include "3rdparty/cdaylward/pathname.h"
#include "appc/schema/common.h"


namespace appc {
namespace schema {


struct Path : StringType<Path> {
  explicit Path(const std::string& path)
  : StringType<Path>(path) {}

  Status validate() const {
    if (pathname::is_absolute(value)) return Success();
    return Error(value + " is not absolute.");
  }
};


} //namespace schema
} //namespace appc
