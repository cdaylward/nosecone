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

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
// TODO decide on term attrs
#include <termios.h>
#include <unistd.h>

#include "appc/image/image.h"
#include "appc/os/mkdir.h"
#include "appc/util/status.h"
#include "appc/util/option.h"

#include "nosecone/errno.h"
#include "nosecone/executor/container/linux.h"

#ifdef __linux__
#include <sys/prctl.h>

namespace nosecone {
namespace executor {
namespace container {
namespace linux {


inline char** c_env_array(const std::map<std::string, std::string>& environment) {
  const size_t env_size = environment.size() + 1;
  char** environment_array = static_cast<char**>(calloc(env_size, sizeof(char*)));
  off_t i = 0;
  for (const auto& pair : environment) {
    const auto assignment = pair.first + "=" + pair.second;
    environment_array[i++] = strdup(const_cast<char*>(assignment.c_str()));
  }
  environment_array[i] = NULL;
  return environment_array;
}


inline char** c_array(const std::vector<std::string>& strings) {
  const size_t array_size = strings.size() + 1;
  char** array = static_cast<char**>(calloc(array_size, sizeof(char*)));
  off_t i = 0;
  for (const auto& str : strings) {
    array[i++] = strdup(const_cast<char*>(str.c_str()));
  }
  array[i] = NULL;
  return array;
}


inline char** c_array(const appc::schema::Exec& exec) {
  std::vector<std::string> strings{};
  for (const auto& arg : exec) {
    strings.push_back(arg.value);
  }
  return c_array(strings);
}


Status Container::Impl::create_rootfs() {
  const auto made_container_root = appc::os::mkdir(rootfs_path, 0755, true);
  if (!made_container_root) {
    std::string where{"Could not create directory for container: "};
    return Error(where + made_container_root.message);
  }

  for (auto& image : images) {
    auto extracted = image.archive.extract_rootfs_to(rootfs_path);
    if (!extracted) {
      std::string where{"Could not create root file system for container: "};
      return Error(where + extracted.message);
    }
  }

  std::cerr << "Created root file system: " << rootfs_path << std::endl;

  return Success();
}


std::string Container::Impl::id() const {
  return uuid;
}


pid_t Container::Impl::pid() const {
  return clone_pid;
}


bool Container::Impl::has_pty() const {
  return pty_created;
}

int Container::Impl::pty_fd() const {
  return pty_master_fd;
}


Status await(const Container& container) {
  const pid_t pid = container.pid();
  if (pid == 0) {
    return Error("Cannot call wait from within container.");
  }
  if (waitpid(pid, NULL, __WALL) == -1) {
    return Errno("Could not wait on container", errno);
  }

  return Success();
}


bool parent_of(const Container& container) {
  return container.pid() > 0;
}


Status Container::Impl::create_pty() {
  pty_master_fd = posix_openpt(O_RDWR|O_NOCTTY|O_CLOEXEC);
  if (pty_master_fd < 0) {
    Errno("Could not create pseudoterminal for container", errno);
  }

  char slave_buff[100];
  if (ptsname_r(pty_master_fd, slave_buff, sizeof(slave_buff) - 1 ) != 0) {
    Errno("Failed to determine tty name", errno);
  }
  pty_slave_name = std::string{slave_buff};

  if (grantpt(pty_master_fd) != 0) {
    Errno("Failed to change tty owner", errno);
  }

  if (unlockpt(pty_master_fd) != 0) {
    Errno("Failed to unlock tty", errno);
  }

  pty_created = true;

  return Success();
}


Status Container::Impl::start() {
  auto app_image = images[0];
  if (!app_image.manifest.app) {
    return Error("Image is not startable, no app specified.");
  }
  auto app = from_some(app_image.manifest.app);

  auto clone_flags = CLONE_NEWIPC |
                     CLONE_NEWPID |
                     CLONE_NEWUTS |
                     CLONE_NEWNS;
  // CLONE_NEWNET? O_o
  // No signal is sent to parent.
  clone_pid = syscall(__NR_clone, clone_flags, NULL);
  if (clone_pid == -1) {
    return Errno("Failed to clone", errno);
  }
  if (clone_pid > 0) return Success("parent");

  if (pty_created) {
    close(pty_master_fd);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    auto pty_slave_fd = open(pty_slave_name.c_str(), O_RDWR);
    if (pty_slave_fd == -1) {
      return Errno("Could not open terminal", errno);
    }
    if (pty_slave_fd != 0) {
      return Errno("Extraneous file handles open when creating terminal", errno);
    }
    dup2(STDIN_FILENO, STDOUT_FILENO);
    dup2(STDIN_FILENO, STDERR_FILENO);
    if (pty_slave_fd > STDERR_FILENO) {
      close(pty_slave_fd);
    }
  }

  if (setsid() == -1) {
    return Errno("Failed to create new session", errno);
  }

  // TODO test
  ioctl(0, TIOCSCTTY, 1);

  // TODO Set PDEATHSIG?

  // Remount / as slave (so we don't pollute mounts but receive host's)
  if (mount(NULL, "/", NULL, MS_SLAVE|MS_REC, NULL) != 0) {
    return Errno("Failed to mount / as slave", errno);
  }
  if (mount(rootfs_path.c_str(), rootfs_path.c_str(), "bind", MS_BIND|MS_REC, NULL) != 0) {
    return Errno("Failed to bind mount rootfs", errno);
  }

  auto proc_dir = pathname::join(rootfs_path, "proc");
  appc::os::mkdir(proc_dir, 0755, true);
  if (mount("proc", proc_dir.c_str(), "proc",  MS_NOSUID|MS_NOEXEC|MS_NODEV, NULL) != 0) {
    return Errno("Failed to mount /proc", errno);
  }

  auto proc_sys_dir = pathname::join(rootfs_path, "proc", "sys");
  appc::os::mkdir(proc_sys_dir, 0755, true);
  if (mount("/proc/sys", proc_sys_dir.c_str(), NULL, MS_BIND, NULL)) {
    return Errno("Failed to bind mount /proc/sys", errno);
  }
  if (mount(NULL, proc_sys_dir.c_str(), NULL, MS_BIND|MS_RDONLY|MS_REMOUNT, NULL)) {
    return Errno("Failed to remount /proc/sys read-only", errno);
  }

  auto sys_dir = pathname::join(rootfs_path, "sys");
  appc::os::mkdir(sys_dir, 0755, true);
  if (mount("sysfs", sys_dir.c_str(), "sysfs", MS_RDONLY|MS_NOSUID|MS_NOEXEC|MS_NODEV, NULL)) {
    return Errno("Failed to mount sysfs /sys", errno);
  }

  auto dev_dir = pathname::join(rootfs_path, "dev");
  appc::os::mkdir(dev_dir, 0755, true);
  if (mount("tmpfs", dev_dir.c_str(), "tmpfs", MS_NOSUID|MS_STRICTATIME, "mode=755")) {
    return Errno("Failed to mount tmpfs /dev", errno);
  }

  auto pts_dir = pathname::join(rootfs_path, "dev", "pts");
  appc::os::mkdir(pts_dir, 0755, true);
  if (mount("devpts", pts_dir.c_str(), "devpts", MS_NOSUID|MS_NOEXEC, NULL)) {
    return Errno("Failed to mount devpts /dev/pts", errno);
  }

  auto new_stdin = pathname::join(rootfs_path, "dev", "stdin");
  auto new_stdout = pathname::join(rootfs_path, "dev", "stdout");
  auto new_stderr = pathname::join(rootfs_path, "dev", "stderr");
  // TODO Test status
  symlink("/proc/self/fd/0", new_stdin.c_str());
  symlink("/proc/self/fd/1", new_stdout.c_str());
  symlink("/proc/self/fd/2", new_stderr.c_str());

  // copy dev nodes
  // set seccomp
  // mknod dev/console
  // add kmsg?
  // locale?
  // timezone?
  // set resolv.conf
  // bind mounts RW
  // bind mounts RO
  // mount tmpfs


  // Signal parent to enforce cgroups

  if (chdir(rootfs_path.c_str()) != 0) {
    return Errno("Failed to chdir to rootfs", errno);
  }

  if (mount(rootfs_path.c_str(), "/", NULL, MS_MOVE, NULL) != 0) {
    return Errno("Failed to move mount /", errno);
  }

  if (chroot(".") != 0) {
    return Errno("chroot failed", errno);
  }

  if (chdir("/") != 0) {
    return Errno("chdir failed", errno);
  }

  // Set up network
  if (sethostname(uuid.c_str(), uuid.length()) != 0) {
    return Errno("Could not set hostname", errno);
  }
  // Drop Capabilities
  // Set SE Linux context

  umask(0022);

  gid_t gid = stoi(app.group.value);
  if (setgid(gid) != 0) {
    return Errno("setgid failed", errno);
  }
  uid_t uid = stoi(app.user.value);
  if (setuid(uid) != 0) {
    return Errno("setuid failed", errno);
  }

  std::map<std::string, std::string> environment{};
  environment["HOME"] = "/"; // TODO
  environment["LOGNAME"] = app.user.value; // TODO
  environment["PATH"] = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
  environment["SHELL"] = "/bin/bash"; // TODO
  environment["USER"] = app.user.value;
  char* term_type = getenv("TERM");
  if (term_type != NULL) {
    environment["TERM"] = term_type;
  }
  if (app.environment) {
    for (const auto& pair : from_some(app.environment)) {
      environment[pair.name] = pair.value;
    }
  }
  char** environment_array = c_env_array(environment);

  // Enmapen schema array types.
  char** pre_start_arguments_array = NULL;
  if (app.event_handlers) {
    const auto handlers = from_some(app.event_handlers);
    for (const auto& handler : handlers) {
      if (handler.name.value == "pre-start") {
        pre_start_arguments_array = c_array(handler.exec);
        break;
      }
    }
  }
  if (pre_start_arguments_array) {
    auto prestart_pid = fork();
    if (prestart_pid) {
      waitpid(prestart_pid, NULL, 0);
    } else {
      if (execvpe(pre_start_arguments_array[0],
                  pre_start_arguments_array,
                  environment_array) == -1) {
        return Errno("execvpe failed: ", errno);
      }
    }
  }

  char** exec_arguments_array = c_array(app.exec);
  // REM free allocated arrays if exec isn't called.
  // REM update container PID code
  auto app_pid = fork();
  if (app_pid) {
    waitpid(app_pid, NULL, 0);
  } else {
    if (execvpe(exec_arguments_array[0], exec_arguments_array, environment_array) == -1) {
      return Errno("execvpe failed: ", errno);
    }
  }

  char** post_stop_arguments_array = NULL;
  if (app.event_handlers) {
    const auto handlers = from_some(app.event_handlers);
    for (const auto& handler : handlers) {
      if (handler.name.value == "post-stop") {
        post_stop_arguments_array = c_array(handler.exec);
        break;
      }
    }
  }
  if (post_stop_arguments_array) {
    auto poststart_pid = fork();
    if (poststart_pid) {
      waitpid(poststart_pid, NULL, 0);
    } else {
      if (execvpe(post_stop_arguments_array[0],
                  post_stop_arguments_array,
                  environment_array) == -1) {
        return Errno("execvpe failed: ", errno);
      }
    }
  }

  return Success("child");
}


} // namespace linux
} // namespace container
} // namespace executor
} // namespace nosecone

#endif
