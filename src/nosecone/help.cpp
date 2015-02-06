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
#include <iomanip>
#include <string>
#include <map>
#include <tuple>

#include "nosecone/help.h"


using namespace nosecone;


extern Dispatch dispatch;


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


void unknown_command(const std::string& command) {
  std::cerr << "Unknown command: " << command << std::endl << std::endl;
  print_usage(dispatch.commands);
}
