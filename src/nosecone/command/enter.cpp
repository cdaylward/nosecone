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

#include <sched.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include "3rdparty/cdaylward/pathname.h"
#include "appc/util/try.h"

#include "nosecone/command/enter.h"
#include "nosecone/executor/status.h"
#include "nosecone/help.h"


namespace nosecone {
namespace command {


int perform_enter(const Arguments& args) {
  if (args.ordered_args.size() < 1) {
    std::cerr << "Missing argument: <container ID>" << std::endl << std::endl;
    print_help(enter);
    return EXIT_FAILURE;
  }

  // TODO, not necessarily true. Just preemptive for explaining permission errors
  // for common /proc permissions.
  if (geteuid() != 0) {
    std::cerr << "Containers can only be entered by root." << std::endl;
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
  const auto pid = std::to_string(status.pid);


  // TODO add user and network namespaces

  //const auto user_fd = open(pathname::join("/proc", pid, "ns", "user").c_str(), O_RDONLY);
  //if (setns(user_fd, CLONE_NEWUSER) != 0) {
  //  std::cerr << "Could not associate with user namespace: " << strerror(errno) << std::endl;
  //  return EXIT_FAILURE;
  //}

  const auto ipc_fd = open(pathname::join("/proc", pid, "ns", "ipc").c_str(), O_RDONLY);
  if (ipc_fd < 0 || setns(ipc_fd, CLONE_NEWIPC) != 0) {
    std::cerr << "Could not associate with IPC namespace: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  close(ipc_fd);

  const auto uts_fd = open(pathname::join("/proc", pid, "ns", "uts").c_str(), O_RDONLY);
  if (uts_fd < 0 || setns(uts_fd, CLONE_NEWUTS) != 0) {
    std::cerr << "Could not associate with UTS namespace: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  close(uts_fd);

  //const auto net_fd = open(pathname::join("/proc", pid, "ns", "net").c_str(), O_RDONLY);
  //if (setns(net_fd, CLONE_NEWNS) != 0) {
  //  std::cerr << "Could not associate with network namespace: " << strerror(errno) << std::endl;
  //  return EXIT_FAILURE;
  //}

  const auto pid_fd = open(pathname::join("/proc", pid, "ns", "pid").c_str(), O_RDONLY);
  if (pid_fd < 0 || setns(pid_fd, CLONE_NEWPID) != 0) {
    std::cerr << "Could not associate with PID namespace: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  close(pid_fd);

  const auto mnt_fd = open(pathname::join("/proc", pid, "ns", "mnt").c_str(), O_RDONLY);
  if (mnt_fd < 0 || setns(mnt_fd, CLONE_NEWNS) != 0) {
    std::cerr << "Could not associate with mount namespace: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  close(mnt_fd);

  pid_t child_pid = fork();
  // Set uid, gid? Leave as root with --admin?

  // FIXME
  if (child_pid) {
    waitpid(child_pid, NULL, __WALL);
    return EXIT_SUCCESS;
  }

  // TODO plumb
  execlp("/bin/bash", "/bin/bash", "-i", NULL);

  return EXIT_SUCCESS;
}


} // namespace command
} // namespace nosecone
