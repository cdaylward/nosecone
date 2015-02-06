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

#include "3rdparty/nlohmann/json.h"
#include "appc/image/image.h"
#include "appc/schema/image.h"
#include "appc/util/try.h"
#include "nosecone/config.h"
#include "nosecone/help.h"
#include "nosecone/validate.h"


using namespace appc;
using Json = nlohmann::json;

extern nosecone::Config config;


namespace nosecone {


int validate(const std::string& filename) {
  image::Image image{filename};

  auto valid_structure = image.validate_structure();
  if (!valid_structure) {
    std::cerr << filename << " is not a valid ACI: " << valid_structure.message << std::endl;
    return EXIT_FAILURE;
  }

  std::cerr << "ACI structure is OK." << std::endl;

  auto manifest_text = image.manifest();
  if (!manifest_text) {
    std::cerr << "Could not retrieve manifest from ACI: ";
    std::cerr << manifest_text.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }

  Json manifest_json;
  try {
    manifest_json = Json::parse(from_result(manifest_text));
  } catch (const std::invalid_argument& err) {
    std::cerr << "Manifest is invalid JSON: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  auto manifest = schema::ImageManifest::from_json(manifest_json);
  if (!manifest) {
    std::cerr << "Could not parse: " << manifest.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }

  auto valid = manifest->validate();
  if (!valid) {
    std::cerr << "Manifest is invalid: " << valid.message << std::endl;
    return EXIT_FAILURE;
  }

  std::cerr << "ACI manifest is OK." << std::endl;

  return 0;
}


int process_validate_args(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    std::cerr << "Missing argument: <path to image>" << std::endl << std::endl;
    print_help(command::validate);
    return EXIT_FAILURE;
  }
  const std::string filename{args[1]};

  return validate(filename);
}


} // namespace nosecone
