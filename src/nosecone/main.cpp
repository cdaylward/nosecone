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

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "nosecone/config.h"
#include "nosecone/command/enter.h"
#include "nosecone/command/fetch.h"
#include "nosecone/command/gc.h"
#include "nosecone/help.h"
#include "nosecone/command/list.h"
#include "nosecone/command/fetch.h"
#include "nosecone/command/run.h"
#include "nosecone/command/status.h"
#include "nosecone/command/validate.h"


using namespace nosecone;

Config config{
  false,
  "/tmp/nosecone/images",
  "/tmp/nosecone/containers"
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

  std::vector<std::string> arguments;
  for (int i = 1; i < args; i++) {
    arguments.emplace_back(argv[i]);
  }

  if (arguments.empty()) {
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  const auto& command = arguments[0];

  if (command == "help" || command == "--help") {
    if (arguments.size() == 2) {
      const auto& topic = arguments[1];
      if (dispatch.commands.find(topic) == dispatch.commands.end()) {
        unknown_command(topic);
        return EXIT_FAILURE;
      }
      print_help(dispatch.commands[topic]);
      return EXIT_SUCCESS;
    }
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  if (dispatch.commands.find(command) == dispatch.commands.end()) {
    unknown_command(command);
    return EXIT_FAILURE;
  }

  return dispatch.run(command, std::vector<std::string>{arguments.begin() + 1, arguments.end()});
}
