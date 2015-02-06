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

#pragma once

#include "nosecone/command.h"


namespace nosecone {
namespace command {


int perform_fetch(const Arguments& args);


const Command fetch{
  "fetch",
  "Fetches an image and stores it locally.",
  "Usage: nscn fetch <app name> [<label name>:<label value> ...]\n\n"
  "version, os, and arch labels default to 1.0.0, linux, and amd64 respectively.\n"
  "The following commands are equivalent:\n"
  "$ nscn fetch nosecone.net/example/worker\n"
  "$ nscn fetch nosecone.net/example/worker version:1.0.0 os:linux arch:amd64",
  perform_fetch
};


} // namespace command
} // namespace nosecone
