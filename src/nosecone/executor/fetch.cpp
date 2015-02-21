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
#include "appc/os/mkdir.h"

#include "nosecone/config.h"
#include "nosecone/executor/fetch.h"
#include "nosecone/executor/validate.h"
#include "nosecone/help.h"


namespace nosecone {
namespace executor {


Try<URI> fetch(const appc::discovery::Name& name, const appc::discovery::Labels& labels)
{
  const auto made_image_dir = appc::os::mkdir(config.images_path, 0755, true);
  if (!made_image_dir) {
    return Failure<URI>(std::string{"Could not make directory for images: "} + config.images_path);
  }

  const std::string storage_base = "file://" + config.images_path;

  const auto local_strategy = appc::discovery::strategy::local::StrategyBuilder()
                                .with_storage_base_uri(storage_base)
                                .build();

  const auto simple_strategy = appc::discovery::strategy::simple::StrategyBuilder()
                                 .with_storage_base_uri(storage_base)
                                 .build();

  auto provider = appc::discovery::ImageProvider({
    from_result(local_strategy),
    from_result(simple_strategy)
  });

  const auto image_location = provider.get(name, labels);

  if (!image_location) {
    return Failure<URI>(std::string{"Failed to retrieve image for "} + name + ": " +
        image_location.failure_reason());
  }

  return image_location;
}


Try<Images>
fetch_and_validate(const appc::discovery::Name& name,
                   const appc::discovery::Labels& labels,
                   const bool with_dependencies,
                   Images dependencies) {
  auto image_uri = fetch(name, labels);
  if (!image_uri) return Failure<Images>(image_uri.failure_reason());

  auto image_path = uri_file_path(from_result(image_uri));

  auto valid_structure = validate_structure(image_path);
  if (!valid_structure) return Failure<Images>(valid_structure.message);

  auto valid_image_try = get_validated_image(image_path);
  if (!valid_image_try) return Failure<Images>(valid_image_try.failure_reason());

  auto valid_image = from_result(valid_image_try);

  std::cerr << "Validated: " << pathname::base(image_path) << " OK" << std::endl;

  dependencies.push_back(valid_image);

  // TODO is depth-first ok?
  if (with_dependencies && valid_image.manifest.dependencies) {
    for (const auto& dependency : from_some(valid_image.manifest.dependencies)) {
      std::cerr << "Dependency: " << valid_image.manifest.name.value << " requires ";
      std::cerr << dependency.app_name.value << std::endl;
      Labels dependency_labels = config.default_labels;
      if (dependency.labels) {
        dependency_labels = from_some(dependency.labels);
      }
      auto downstream_image_try = fetch_and_validate(dependency.app_name,
                                                     dependency_labels,
                                                     true,
                                                     dependencies);
      if (!downstream_image_try) {
        return Failure<Images>(downstream_image_try.failure_reason());
      }
      auto downstream_images = from_result(downstream_image_try);
      std::copy(downstream_images.begin(),
                downstream_images.end(),
                std::back_inserter(dependencies));
    }
  }

  return Result(dependencies);
}


} // namespace executor
} // namespace nosecone
