# Nose Cone

## Overview

Nose Cone is a C++ [App Container](https://github.com/appc/spec) implementation that
uses the [libappc](https://github.com/cdaylward/libappc) App Container library.

## Getting Started

Requires functional std::regex (If using gcc, >= 4.9)

1. Bootstrap it (download and build dependencies): `./bootstrap.sh`
2. Build Nose Cone (nscn): `./build.sh`
3. Run `./bin/nscn help`

## Example (run)

`nscn run` is not complete, however it will perform simple discovery, cache images
locally, validate them, create a rootfs, and execute the app inside a new app container.

nosecone.net/example/test is an existing example app, the following example should work (-stdout
reads the app container's pty and writes to stdout):

```
$ sudo ./bin/nscn run -stdout nosecone.net/example/test
Resolved: nosecone.net/example/test -> file:///tmp/nosecone/images/nosecone.net/example/test-1.0.0-linux-amd64.aci
Using: file:///tmp/nosecone/images/nosecone.net/example/test-1.0.0-linux-amd64.aci
Validated: test-1.0.0-linux-amd64.aci OK
Dependency: nosecone.net/example/test requires nosecone.net/example/tinycentos7
Resolved: nosecone.net/example/tinycentos7 -> file:///tmp/nosecone/images/nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
Using: file:///tmp/nosecone/images/nosecone.net/example/tinycentos7-1.0.0-linux-amd64.aci
Validated: tinycentos7-1.0.0-linux-amd64.aci OK
Container ID: 08f2d475-cf46-4398-a820-ddcd9e59ba0e
Created root file system: /tmp/nosecone/containers/08f2d475-cf46-4398-a820-ddcd9e59ba0e/rootfs
Container started, PID: 9238
--- 8< ---
Hello World!
---
1706857    4 drwxrwxr-x   9 0        0            4096 Feb 17 18:59 /
      1    0 dr-xr-xr-x  13 0        0               0 Nov  2 01:31 /sys
1706862    4 drwxrwxr-x   2 0        0            4096 Feb 13 21:34 /lib64
2383636    0 drwxr-xr-x   3 0        0             120 Feb 17 18:59 /dev
1706858    4 drwxrwxr-x   6 0        0            4096 Feb 17 18:59 /usr
1706921    4 drwxrwxr-x   2 0        0            4096 Feb 16 23:50 /etc
      1    0 dr-xr-xr-x 107 0        0               0 Feb 17 18:59 /proc
1706934    0 -rw-r--r--   1 500      500             0 Feb 17 18:59 /hello_world
1706922    4 drwxrwxr-x   2 0        0            4096 Feb 13 06:52 /bin
---
uid=500 gid=500 groups=0
---
TERM=xterm-256color
SHELL=/bin/bash
WE_ARE_TESTING=true
USER=500
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
PWD=/
SHLVL=1
HOME=/
LOGNAME=500
_=/usr/bin/env
---
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
500          1  0.0  0.1   9512  2576 pts/3    Ss+  18:59   0:00 /bin/bash /usr/local/bin/test
500         16  0.0  0.0  17648   636 pts/3    R+   18:59   0:00 /bin/ps axufw


$ ./bin/nscn list
Container ID                           Created Date            PID     Has PTY Status
02bf6bf2-a624-43ec-b30a-1f41f4c8ead3   2015-02-20T06:58:07.0Z    18036    true EXITED
40c1c0ea-9c71-43fd-bb29-2f3304c8a18e   2015-02-20T08:09:18.0Z    19097    true RUNNING
d7ed46a5-ed02-45e0-92bf-634793a3949c   2015-02-20T08:48:36.0Z    20101    true RUNNING
58e4ae04-073b-4b8b-95fe-08ac1b19f10b   2015-02-20T08:35:09.0Z    19738    true RUNNING

```

## Status

Very early, experimental development. Based on appc specification ~ 0.2.0 (this will converge)

Commands implemented (this means exists at all, not finished):
- enter    - Enter a running app container.
- fetch    - Fetches an image and stores it locally.
- gc       - Expunge spent app containers.
- list     - List app containers.
- run      - Fetch and execute an app container.
- status   - Display the status of an app container.
- validate - Validate an app container image.

Executor implementation status (list not complete):

- [ ] run
    - [x] Fetch image using simple discovery.
    - [ ] Fetch image using meta discovery on fall-back.
    - [ ] Verify signature of images.
    - [x] Cache image locally.
    - [x] Inspect image for dependencies and fetch them.
    - [x] Overlay images' rootfs onto app container root file system.
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
- [ ] meta-data service

## License

Nose Cone is licensed under the Apache License, Version 2.0.
