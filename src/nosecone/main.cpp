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
#include "nosecone/enter.h"
#include "nosecone/fetch.h"
#include "nosecone/gc.h"
#include "nosecone/list.h"
#include "nosecone/fetch.h"
#include "nosecone/run.h"
#include "nosecone/status.h"


using namespace nosecone;


void print_usage(const std::map<std::string, Command>& commands);
void print_help(const Command& command);
void unknown_command(const std::string& command);


Dispatch dispatch{};


int main(int args, char** argv)
{
  dispatch.register_command(nosecone::command::enter);
  dispatch.register_command(nosecone::command::fetch);
  dispatch.register_command(nosecone::command::gc);
  dispatch.register_command(nosecone::command::list);
  dispatch.register_command(nosecone::command::run);
  dispatch.register_command(nosecone::command::status);

  std::vector<std::string> arguments;
  for (int i = 1; i < args; i++) {
    arguments.emplace_back(argv[i]);
  }

  if (arguments.empty()) {
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  const auto& command = arguments[0];

  if (command == "help") {
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

  // FIXME
  const std::string mkdir_images = "mkdir -p -- " + config::images_path;
  system(mkdir_images.c_str());
  const std::string mkdir_containers = "mkdir -p -- " + config::containers_path;
  system(mkdir_containers.c_str());

  return dispatch.run(command, arguments);
}


void unknown_command(const std::string& command) {
  std::cerr << "Unknown command: " << command << std::endl << std::endl;
  print_usage(dispatch.commands);
}


void print_usage(const std::map<std::string, Command>& commands) {
  std::cout << "nscn - nose cone, an app container executor." << std::endl << std::endl;
  std::cout << "Usage: nscn [flags] <command> [command options]" << std::endl;
  std::cout << "  Flags:" << std::endl;
  std::cout << "    " << std::setw(6) << std::left << "-v";
  std::cout << " - " << "Be verbose." << std::endl;
  std::cout << "  Commands:" << std::endl;
  for (const auto& pair : commands) {
    const auto& name = pair.first;
    const auto& command = pair.second;
    std::cout << "    " << std::setw(6) << std::left << name;
    std::cout << " - " << command.description << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Run `nscn help <command>` for more details on <command>." << std::endl;
}


void print_help(const Command& command) {
  std::cout << command.name << " - " << command.description << std::endl;
  std::cout << std::endl;
  std::cout << command.help_text << std::endl;
  std::cout << std::endl;
}
