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

#include <cerrno>
#include <algorithm>
#include <iostream>

#include "appc/schema/image.h"
#include "nosecone/help.h"
#include "nosecone/config.h"
#include "nosecone/executor/run.h"
#include "nosecone/executor/fetch.h"
#include "nosecone/executor/validate.h"
#include "nosecone/executor/image.h"


extern nosecone::Config config;


namespace nosecone {
namespace executor {


using namespace appc;


using ValidatedImages = std::vector<ValidatedImage>;


Try<ValidatedImages>
fetch_and_validate(const discovery::Name& name,
                   const discovery::Labels& labels,
                   const bool with_dependencies = false,
                   ValidatedImages dependencies = ValidatedImages{}) {
  auto image_uri = fetch(name, labels);
  if (!image_uri) return Failure<ValidatedImages>(image_uri.failure_reason());

  auto image_path = uri_file_path(from_result(image_uri));

  auto valid_structure = validate_structure(image_path);
  if (!valid_structure) return Failure<ValidatedImages>(valid_structure.message);

  auto valid_image_try = get_validated_image(image_path);
  if (!valid_image_try) return Failure<ValidatedImages>(valid_image_try.failure_reason());

  auto valid_image = from_result(valid_image_try);

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
                                                         labels,
                                                         true,
                                                         dependencies);
      if (!downstream_image_try) {
        return Failure<ValidatedImages>(downstream_image_try.failure_reason());
      }
      auto downstream_images = from_result(downstream_image_try);
      std::copy(downstream_images.begin(),
                downstream_images.end(),
                std::back_inserter(dependencies));
    }
  }

  return Result(dependencies);
}


int run(const discovery::Name& name, const discovery::Labels& labels) {
  // FIXME
  const std::string mkdir_containers = "mkdir -p -- " + config.containers_path;
  if (system(mkdir_containers.c_str()) != 0) {
    std::cerr << "Could not create dir for containers: " << strerror(errno) << std::endl;
  }

  auto images_try = fetch_and_validate(name, labels, true);
  if (!images_try) {
    std::cerr << images_try.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }

  auto images = from_result(images_try);

  return EXIT_SUCCESS;
}


} // namespace executor
} // namespace nosecone
