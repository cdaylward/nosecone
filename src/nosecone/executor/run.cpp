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

#include "nosecone/help.h"
#include "nosecone/config.h"
#include "nosecone/executor/run.h"
#include "nosecone/executor/fetch.h"
#include "nosecone/executor/validate.h"


extern nosecone::Config config;


namespace nosecone {
namespace executor {


using namespace appc::discovery;


int run(const Name& name, const Labels& labels) {
  // FIXME
  const std::string mkdir_containers = "mkdir -p -- " + config.containers_path;
  system(mkdir_containers.c_str());

  auto image_uri = fetch(name, labels);
  if (!image_uri) return 1;

  auto image_path = uri_file_path(from_result(image_uri));

  validate(image_path);

  return EXIT_SUCCESS;
}


} // namespace executor
} // namespace nosecone
