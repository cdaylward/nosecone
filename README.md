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
`nscn run` does not yet create a container. It performs simple discovery, caches images locally,
and validates the them.

nosecone.net/example/app is an existing example app, the following example should work:
```
$ ./bin/nscn run nosecone.net/example/app
Resolved: nosecone.net/example/app -> file:///tmp/nosecone/images/nosecone.net/example/app-1.0.0-linux-amd64.aci
Fetched: file:///tmp/nosecone/images/nosecone.net/example/app-1.0.0-linux-amd64.aci
app-1.0.0-linux-amd64.aci OK
Dependency: nosecone.net/example/app requires nosecone.net/example/worker
Resolved: nosecone.net/example/worker -> file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
Fetched: file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
worker-1.0.0-linux-amd64.aci OK
Dependency: nosecone.net/example/app requires nosecone.net/example/database
Resolved: nosecone.net/example/database -> file:///tmp/nosecone/images/nosecone.net/example/database-1.0.0-linux-amd64.aci
Fetched: file:///tmp/nosecone/images/nosecone.net/example/database-1.0.0-linux-amd64.aci
database-1.0.0-linux-amd64.aci OK

$ ./bin/nscn list
Container ID
29D3EEA7-306B-47CC-8576-CCE94E2A3383
8F206A33-2D99-45EE-9026-02ED4E17561C

$ find /tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/usr
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/usr/bin
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/usr/bin/worker
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/usr/sbin
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/usr/sbin/database
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/var
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/var/lib
/tmp/nosecone/containers/29D3EEA7-306B-47CC-8576-CCE94E2A3383/rootfs/var/lib/db


$ actool validate /tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
```

## Status

Very early, experimental development. Based on appc specification ~ 0.2.0 (this will converge)

Command implementation status (this means exists at all, not finished):
- [ ] enter    - Enter a running container.
- [x] fetch    - Fetches an image and stores it locally.
- [ ] gc       - Expunge spent containers.
- [x] list     - List containers.
- [ ] run      - Execute a command in a new container.
- [ ] status   - Display the status of a container.
- [x] validate - Validate an app container image.

Executor implementation status (list not complete):

- [ ] run
    - [x] Fetch image using simple discovery.
    - [ ] Fetch image using meta discovery on fall-back.
    - [x] Cache image locally.
    - [x] Inspect image for dependencies and fetch them.
    - [ ] Overlay images' rootfs to container root file system.
    - [ ] Bind-mount in volumes.
    - [ ] Create container context.
    - [ ] Enter container.
    - [ ] Execute.

## License

Nose Cone is licensed under the Apache License, Version 2.0.
