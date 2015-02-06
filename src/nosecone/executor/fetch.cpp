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

#include "appc/discovery/provider.h"
#include "appc/discovery/strategy/local.h"
#include "appc/discovery/strategy/meta.h"
#include "appc/discovery/strategy/simple.h"

#include "nosecone/config.h"
#include "nosecone/executor/fetch.h"
#include "nosecone/help.h"


extern nosecone::Config config;


namespace nosecone {
namespace executor {


using namespace appc::discovery;


Try<URI> fetch(const Name& name, const Labels& labels)
{
  // FIXME
  const std::string mkdir_images = "mkdir -p -- " + config.images_path;
  system(mkdir_images.c_str());

  const std::string storage_base = "file://" + config.images_path;

  const auto local_strategy = strategy::local::StrategyBuilder()
                                .with_storage_base_uri(storage_base)
                                .build();

  const auto simple_strategy = strategy::simple::StrategyBuilder()
                                 .with_storage_base_uri(storage_base)
                                 .build();

  auto provider = ImageProvider({
    from_result(local_strategy),
    from_result(simple_strategy)
  });

  const auto image_location = provider.get(name, labels);

  if (!image_location) {
    std::cerr << "Failed to retrieve image for " << name << std::endl;
  }

  return image_location;
}


} // namespace executor
} // namespace nosecone
