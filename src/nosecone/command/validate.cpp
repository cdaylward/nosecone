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
#include "nosecone/command/validate.h"
#include "nosecone/executor/validate.h"


namespace nosecone {
namespace command {


int perform_validate(const Arguments& args) {
  if (args.ordered_args.size() < 1) {
    std::cerr << "Missing argument: <path to image>" << std::endl << std::endl;
    print_help(command::validate);
    return EXIT_FAILURE;
  }
  const std::string filename{args.ordered_args[0]};

  auto valid =  executor::validate(filename);
  if (valid) {
    std::cerr << pathname::base(filename) << " OK" << std::endl;
    return EXIT_SUCCESS;
  }

  std::cerr << pathname::base(filename) << " NOT OK " << valid.message << std::endl;
  return EXIT_FAILURE;
}


} // namespace command
} // namespace nosecone
