# Nose Cone

## Overview

Nose Cone is a C++ [Rocket](https://github.com/coreos/rocket) clone that uses the
[libappc](https://github.com/cdaylward/libappc) App Container library.

Nose Cone's interface will differ somewhat from that of Rocket's, however it should
feel similar.

## Getting Started

Requires functional std::regex (If using gcc, >= 4.9)

1. Bootstrap it (download and build dependencies): `./bootstrap.sh`
2. Build Nose Cone (nscn): `./build.sh`
3. Run `./bin/nscn help`

## Example (run)

`nscn run` is not complete, however it will perform simple discovery, cache images
locally, validate the them, create a rootfs, and execute the app inside a new container.

nosecone.net/example/test is an existing example app, the following example should work (-stdout
reads the container's pty and writes to stdout):

```
$ sudo ./bin/nscn run -stdout nosecone.net/example/test
Resolved: nosecone.net/example/test -> file:///tmp/nosecone/images/nosecone.net/example/test-1.0.0-linux-amd64.aci
Fetch failed: file:///tmp/nosecone/images/nosecone.net/example/test-1.0.0-linux-amd64.aci No such file or directory
Resolved: nosecone.net/example/test -> https://nosecone.net/example/test-1.0.0-linux-amd64.aci
Fetched: https://nosecone.net/example/test-1.0.0-linux-amd64.aci
Location: file:///tmp/nosecone/images/nosecone.net/example/test-1.0.0-linux-amd64.aci
test-1.0.0-linux-amd64.aci OK
Dependency: nosecone.net/example/test requires nosecone.net/example/tinycentos7
Resolved: nosecone.net/example/tinycentos7 -> file:///tmp/nosecone/images/nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
Fetch failed: file:///tmp/nosecone/images/nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci No such file or directory
Resolved: nosecone.net/example/tinycentos7 -> https://nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
Fetched: https://nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
Location: file:///tmp/nosecone/images/nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
tinycentos7-1.0.0-linux-amd64.aci OK
Creating container 8ecd98b1-6da8-46c4-b433-93bae3893ec4
Creating rootfs: /tmp/nosecone/containers/8ecd98b1-6da8-46c4-b433-93bae3893ec4/rootfs
Container Started, PID: 6949
Hello World!
---
1705488    4 drwxrwxr-x   9 0        0            4096 Feb 17 08:01 /
      1    0 dr-xr-xr-x  13 0        0               0 Nov  2 01:31 /sys
1705493    4 drwxrwxr-x   2 0        0            4096 Feb 13 21:34 /lib64
2376077    0 drwxr-xr-x   3 0        0             120 Feb 17 08:01 /dev
1705489    4 drwxrwxr-x   6 0        0            4096 Feb 17 08:01 /usr
1705552    4 drwxrwxr-x   2 0        0            4096 Feb 16 23:50 /etc
      1    0 dr-xr-xr-x 106 0        0               0 Feb 17 08:01 /proc
1705565    0 -rw-r--r--   1 500      500             0 Feb 17 08:01 /hello_world
1705553    4 drwxrwxr-x   2 0        0            4096 Feb 13 06:52 /bin
---
uid=500 gid=500 groups=0
---
SHELL=/bin/bash
WE_ARE_TESTING=true
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
PWD=/
SHLVL=1
_=/usr/bin/env
---
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
500          1  0.0  0.1   9512  2520 pts/2    Ss+  08:01   0:00 /bin/bash /usr/local/bin/test
500         16  0.0  0.0  17648   644 pts/2    R+   08:01   0:00 /bin/ps axufw

$ ./bin/nscn list
Container ID
137eb111-77ab-44a3-9a14-e45bc1e4557c
c0bd7e2c-b64b-4212-966d-9044b2a52f2b
41472f9b-f669-4b91-875f-3fde7bdf21ee
8ecd98b1-6da8-46c4-b433-93bae3893ec4
```

## Status

Very early, experimental development. Based on appc specification ~ 0.2.0 (this will converge)

Command implementation status (this means exists at all, not finished):
- [ ] enter    - Enter a running container.
- [x] fetch    - Fetches an image and stores it locally.
- [ ] gc       - Expunge spent containers.
- [x] list     - List containers.
- [x] run      - Execute a command in a new container.
- [ ] status   - Display the status of a container.
- [x] validate - Validate an app container image.

Executor implementation status (list not complete):

- [ ] run
    - [x] Fetch image using simple discovery.
    - [ ] Fetch image using meta discovery on fall-back.
    - [x] Cache image locally.
    - [x] Inspect image for dependencies and fetch them.
    - [x] Overlay images' rootfs onto container root file system.
    - [x] Create pseudo-terminal.
    - [x] Create new mount, IPC, PID, and UTS namespace.
    - [ ] Create new network namespace.
    - [x] Create /proc
    - [x] Create /proc/sys
    - [x] Create /dev as tmpfs
    - [x] Create /dev/pts
    - [ ] Create /tmp tmpfs
    - [ ] Create common / needed device nose.
    - [ ] Set seccomp
    - [ ] Bind mount RW volumes.
    - [ ] Bind mount RO volumes.
    - [ ] Drop capabilities.
    - [ ] Configure CGroups.
    - [ ] Set locale?
    - [ ] Set timezone?
    - [ ] Set resolve.conf
    - [x] Set hostname (but needs plumbing)
    - [ ] Set up network
    - [x] Set umask
    - [x] Set UID/GID
    - [x] Set environment (defaults + manifest only)
    - [ ] Execute pre-start hook.
    - [x] Execute app.
    - [ ] Execute post-stop hook.

## License

Nose Cone is licensed under the Apache License, Version 2.0.
