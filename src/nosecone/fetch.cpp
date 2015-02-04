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

#include "nosecone/fetch.h"


namespace nosecone {


using namespace appc::discovery;


int perform_fetch(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    std::cerr << "Missing <app name>" << std::endl;
    return 1;
  }
  const Name name{args[1]};

  const Labels labels{
    {"os", "linux"},
    {"version", "0.0.1"},
    {"arch", "x86_64"}
  };

  // By configuring the local strategy and the simple strategy with the same
  // storage location, the local strategy functions as a cache.
  // (currently, the directory in the base uri must exist)

  const auto local_strategy = strategy::local::StrategyBuilder()
                                .with_storage_base_uri("file:///tmp/images")
                                .build();

  const auto simple_strategy = strategy::simple::StrategyBuilder()
                                 .with_storage_base_uri("file:///tmp/images")
                                 .build();

  auto provider = ImageProvider({
    from_result(local_strategy),
    from_result(simple_strategy)
  });

  const auto image_location = provider.get(name, labels);

  if (!image_location) {
    std::cerr << "Failed to retrieve image for " << name << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << from_result(image_location) << std::endl;

  return 0;
}


} // namespace nosecone
