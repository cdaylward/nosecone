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

#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <tuple>

#include "nosecone/command/enter.h"
#include "nosecone/command/fetch.h"
#include "nosecone/command/fetch.h"
#include "nosecone/command/gc.h"
#include "nosecone/command/list.h"
#include "nosecone/command/run.h"
#include "nosecone/command/status.h"
#include "nosecone/command/validate.h"
#include "nosecone/config.h"
#include "nosecone/help.h"


using namespace nosecone;

Config config{
  false,
  "/tmp/nosecone/images",
  "/tmp/nosecone/containers",
  {
    {"os", "linux"},
    {"version", "1.0.0"},
    {"arch", "amd64"}
  }
};

Dispatch dispatch{};


int main(int args, char** argv)
{
  dispatch.register_command(nosecone::command::enter);
  dispatch.register_command(nosecone::command::fetch);
  dispatch.register_command(nosecone::command::gc);
  dispatch.register_command(nosecone::command::list);
  dispatch.register_command(nosecone::command::run);
  dispatch.register_command(nosecone::command::status);
  dispatch.register_command(nosecone::command::validate);

  std::vector<std::string> argvs;
  for (int i = 1; i < args; i++) {
    argvs.emplace_back(argv[i]);
  }

  const auto& arguments = separate_flags(argvs);

  if (arguments.has_flag("help")) {
    print_usage(dispatch.commands);
    return EXIT_SUCCESS;
  }

  if (arguments.has_flag("v")) {
    config.verbose = true;
  }

  if (arguments.ordered_args.empty()) {
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  const auto& command = arguments.ordered_args[0];

  if (command == "help") {
    if (arguments.ordered_args.size() == 2) {
      const auto& topic = arguments.ordered_args[1];
      if (dispatch.commands.find(topic) == dispatch.commands.end()) {
        unknown_command(topic);
        return EXIT_FAILURE;
      }
      print_help(dispatch.commands[topic]);
      return EXIT_SUCCESS;
    }
    print_usage(dispatch.commands);
    return EXIT_SUCCESS;
  }

  if (dispatch.commands.find(command) == dispatch.commands.end()) {
    unknown_command(command);
    return EXIT_FAILURE;
  }

  auto command_options = separate_flags(std::vector<std::string>{arguments.ordered_args.begin() + 1,
                                                                 arguments.ordered_args.end()});
  return dispatch.run(command, command_options);
}
