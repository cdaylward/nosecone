# Nose Cone

## Overview

Nose Cone is a C++ [Rocket](https://github.com/coreos/rocket) clone that uses the [libappc](https://github.com/cdaylward/libappc) App Container library.

## Getting Started

Requires functional std::regex (If using gcc, >= 4.9)

1. Bootstrap it (download and build dependencies): `./bootstrap.sh`
2. Build Nose Cone (nscn): `./build.sh`
3. Run `./bin/nscn help`

## Example (fetch)
`nscn fetch` performs simple discovery and stores images in /tmp/nosecone/images

nosecone.net/example/worker is an existing example app, the following example should work:
```
$ ./bin/nscn fetch nosecone.net/example/worker
Resolved: nosecone.net/example/worker -> file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
Fetch failed: file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci No such file or directory
Resolved: nosecone.net/example/worker -> https://nosecone.net/example/worker-1.0.0-linux-amd64.aci
*   Trying 173.230.146.12...
* Connected to nosecone.net (173.230.146.12) port 443 (#0)
* Connection #0 to host nosecone.net left intact
Fetched: file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
file:///tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci

$ ls -la /tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
-rw-r--r--  1 charlesa  users  721 Feb  5 10:05 /tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci

$ actool validate /tmp/nosecone/images/nosecone.net/example/worker-1.0.0-linux-amd64.aci
```

## Status

Very early, experimental development:

Command implementation status:
- [ ] enter - Enter a running container.
- [x] fetch  - Fetches an image and stores it locally.
- [ ] gc     - Collect garbage.
- [ ] list   - List containers.
- [ ] run    - Execute a command in a new container.
- [ ] status - Display the status of a container.

## License

Nose Cone is licensed under the Apache License, Version 2.0.
