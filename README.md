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
`nscn run` does not yet create a container, it performs simple discovery, caches images in
/tmp/nosecone/images, and validates the them.

nosecone.net/example/worker is an existing example app, the following example should work:
```
$ ./bin/nscn run nosecone.net/example/worker
Resolved: nosecone.net/example/worker -> file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
Fetch failed: file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci No such file or directory
Resolved: nosecone.net/example/worker -> https://nosecone.net/example/worker-1.0.0-linux-amd64.aci
Fetched: https://nosecone.net/example/worker-1.0.0-linux-amd64.aci
Location: file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
worker-1.0.0-linux-amd64.aci OK

$ actool validate /tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
```

## Status

Very early, experimental development. Based on appc specification ~ 0.2.0 (this will converge)

Command implementation status:
- [ ] enter    - Enter a running container.
- [x] fetch    - Fetches an image and stores it locally.
- [ ] gc       - Expunge spent containers.
- [ ] list     - List containers.
- [ ] run      - Execute a command in a new container.
- [ ] status   - Display the status of a container.
- [x] validate - Validate an app container image.

Executor implementation status (list not complete):

- [ ] run
    - [x] Fetch image using simple discovery.
    - [ ] Fetch image using meta discovery on fall-back.
    - [x] Cache image locally.
    - [ ] Inspect image for dependencies and fetch them.
    - [ ] Overlay images' rootfs to container root file system.
    - [ ] Create container context.
    - [ ] Enter container.
    - [ ] Execute.

## License

Nose Cone is licensed under the Apache License, Version 2.0.
