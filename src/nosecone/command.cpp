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

#include "nosecone/command.h"


namespace nosecone {


Arguments separate_flags(const std::vector<std::string>& args) {
  Arguments sep{};
  bool flags_finished = false;
  for (const auto& arg : args) {
    if (!flags_finished && arg == "--") {
      flags_finished = true;
    } else if (!flags_finished && arg.find("-") == 0) {
      sep.flags.push_back(arg.substr(1));
    } else {
      flags_finished = true;
      sep.ordered_args.push_back(arg);
    }
  }
  return sep;
}


} // namespace nosecone
