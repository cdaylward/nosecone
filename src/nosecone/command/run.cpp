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

#include "appc/discovery/types.h"
#include "nosecone/help.h"
#include "nosecone/config.h"
#include "nosecone/command/run.h"
#include "nosecone/executor/run.h"


extern nosecone::Config config;


namespace nosecone {
namespace command {


using namespace appc::discovery;


int perform_run(const Arguments& args) {
  if (args.ordered_args.size() < 1) {
    std::cerr << "Missing argument: <app name>" << std::endl << std::endl;
    print_help(command::run);
    return EXIT_FAILURE;
  }

  const Name name{args.ordered_args[0]};

  Labels labels = config.default_labels;

  if (args.ordered_args.size() > 1) {
    for (auto iter = args.ordered_args.begin() + 1; iter != args.ordered_args.end(); iter++) {
      auto& label_set = *iter;
      auto delim = label_set.find(":");
      if (delim == std::string::npos ||
          delim == label_set.length() - 1) {
        std::cerr << "Additional argument that isn't a label: " << label_set << std::endl;
        return EXIT_FAILURE;
      }
      auto name = label_set.substr(0, delim);
      auto value = label_set.substr(delim + 1);
      labels[name] = value;
    }
  }

  return executor::run(name, labels);
}


} // namespace command
} // namespace nosecone
