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
int unknown_command(const std::string& command);


Dispatch dispatch{};


int main(int args, char** argv)
{
  bool verbose;

  dispatch.register_command(nosecone::enter);
  dispatch.register_command(nosecone::fetch);
  dispatch.register_command(nosecone::gc);
  dispatch.register_command(nosecone::list);
  dispatch.register_command(nosecone::run);
  dispatch.register_command(nosecone::status);

  for (int c = getopt(args, argv, "v"); c != -1; c = getopt(args, argv, "v")) {
    switch (c) {
      case 'v':
        verbose = true;
        break;
      default:
        print_usage(dispatch.commands);
        return EXIT_FAILURE;
    }
  }

  std::vector<std::string> ordered;
  for (int ind = optind; ind < args; ind++) {
    ordered.emplace_back(argv[ind]);
  }

  if (ordered.empty()) {
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  const auto& command = ordered[0];

  if (command == "help") {
    if (ordered.size() == 2) {
      const auto& topic = ordered[1];
      if (dispatch.commands.find(topic) == dispatch.commands.end()) {
        return unknown_command(topic);
      }
      print_help(dispatch.commands[topic]);
      return EXIT_SUCCESS;
    }
    print_usage(dispatch.commands);
    return EXIT_FAILURE;
  }

  if (dispatch.commands.find(command) == dispatch.commands.end()) {
    return unknown_command(command);
  }

  return dispatch.run(command, 0, NULL);
}


int unknown_command(const std::string& command) {
  std::cerr << "Unknown command: " << command << std::endl << std::endl;
  print_usage(dispatch.commands);
  return EXIT_FAILURE;
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
