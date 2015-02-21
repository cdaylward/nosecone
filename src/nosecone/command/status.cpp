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

#include "appc/util/try.h"

#include "nosecone/command/status.h"
#include "nosecone/help.h"
#include "nosecone/executor/status.h"


namespace nosecone {
namespace command {


int perform_status(const Arguments& args) {
  if (args.ordered_args.size() < 1) {
    std::cerr << "Missing argument: <container ID>" << std::endl << std::endl;
    print_help(status);
    return EXIT_FAILURE;
  }

  const auto id = args.ordered_args[0];
  const auto status_try = executor::container_status(id);
  if (!status_try) {
    std::cerr << "Could not retrieve container status: ";
    std::cerr << status_try.failure_reason() << std::endl;
    return EXIT_FAILURE;
  }
  const auto status = from_result(status_try);

  std::cout << "ID: " << status.id << std::endl;
  const auto created_time = status.created_time;
  char time[100];
  strftime(time, 99, "%Y-%m-%dT%H:%M:%S.0Z", gmtime(&created_time));
  std::cout << "Created: " << time << std::endl;
  std::cout << "PID: " << status.pid << std::endl;
  std::cout << "PTY: " << std::boolalpha << status.has_pty << std::endl;
  std::cout << "Status: " << (status.running ? "RUNNING" : "EXITED") << std::endl;

  return EXIT_SUCCESS;
}


} // namespace command
} // namespace nosecone
