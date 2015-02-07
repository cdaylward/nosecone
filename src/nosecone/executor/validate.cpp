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

#include "3rdparty/cdaylward/pathname.h"
#include "appc/image/image.h"
#include "appc/schema/image.h"
#include "appc/util/try.h"
#include "nosecone/config.h"
#include "nosecone/executor/validate.h"


extern nosecone::Config config;


namespace nosecone {
namespace executor {


using namespace appc;


Status validate(const std::string& filename) {
  auto valid_structure = validate_structure(filename);
  if (!valid_structure) return valid_structure;

  auto manifest = get_validated_manifest(filename);
  if (!manifest) return Invalid(manifest.failure_reason());

  return Success();
}


Status validate_structure(const std::string& filename) {
  image::Image image{filename};

  auto valid_structure = image.validate_structure();
  if (!valid_structure) {
    return Invalid(filename + " is not a valid ACI: " + valid_structure.message);
  }

  return Success();
}

Try<schema::ImageManifest> get_validated_manifest(const std::string& filename) {
  using Json = appc::schema::Json;

  image::Image image{filename};

  auto manifest_text = image.manifest();
  if (!manifest_text) {
    return Failure<schema::ImageManifest>(
        std::string{"Could not retrieve manifest from ACI: "} + manifest_text.failure_reason());
  }

  Json manifest_json;
  try {
    manifest_json = Json::parse(from_result(manifest_text));
  } catch (const std::invalid_argument& err) {
    return Failure<schema::ImageManifest>(std::string{"Manifest is invalid JSON: "} + err.what());
  }

  auto manifest = schema::ImageManifest::from_json(manifest_json);
  if (!manifest) {
    return Failure<schema::ImageManifest>(
        std::string{"Could not parse: "} + manifest.failure_reason());
  }

  auto valid = manifest->validate();
  if (!valid) {
    return Failure<schema::ImageManifest>(
        std::string{"Manifest is invalid: "} + valid.message);
  }

  std::cerr << pathname::base(filename) << " OK" << std::endl;

  return manifest;
}


} // namespace executor
} // namespace nosecone
