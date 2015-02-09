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

#include "appc/schema/image.h"
#include "appc/util/status.h"
#include "nosecone/executor/image.h"


namespace nosecone {
namespace executor {


using namespace appc;


int validate(const std::string& filename);
Status validate_structure(const std::string& filename);
Try<Image> get_validated_image(const std::string& filename);


} // namespace executor
} // namespace nosecone
