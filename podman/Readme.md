# Build and release sources from Podman container

This readme explains how and why to use Podman containers.

- [Build and release sources from Podman container](#build-and-release-sources-from-podman-container)
  - [Why use Podman for this project](#why-use-podman-for-this-project)
  - [Features](#features)
  - [Integrated linux distributions](#integrated-linux-distributions)
  - [Installation](#installation)
  - [How to use podmanrun.sh](#how-to-use-podmanrunsh)
    - [Use cases](#use-cases)
  - [Remarks](#remarks)
  - [Todo](#todo)
  - [To contribute](#to-contribute)

## Why use Podman for this project

- Each distribution uses different package versions, by testing the most used distributions, it is easier to trace the regressions between each release of **pvsneslib**.
- You don't want to install all the development packages on your OS but you want to be able to compile the code and test by yourself.
- The Dockerfiles serve as documentation if you want to test the compilation locally on your distribution but you don't know which packages to install.

### Why Podman over Docker?

- **Rootless by default**: No daemon running as root, better security model
- **No service required**: No `systemctl enable/start` needed - just run `podman`
- **No group membership**: No need to add users to a special group
- **Daemonless**: Each container runs as a child process, not managed by a central daemon
- **Docker-compatible**: Uses the same Dockerfile format and similar CLI commands

## Features

- Compile **pvsneslib** tools from different linux distribution versions.
- Create ready-to-use releases in zip form
- Check that pvsneslib compiles correctly on different linux distributions, and thus test different versions of tools and libraries used to compile it.

> There is no relationship between the version of Linux you are using and the type of container used.

## Integrated linux distributions

- ![Cent OS](https://img.shields.io/badge/cent%20os-002260?style=for-the-badge&logo=centos&logoColor=F0F0F0)
- ![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
- ![Fedora](https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white)
- ![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)

All these distributions are in latest stable version.

## Installation

Install Podman on your distribution:

### Fedora / CentOS / RHEL

```bash
sudo dnf install podman
```

### Debian / Ubuntu

```bash
sudo apt-get update
sudo apt-get install podman
```

### macOS

```bash
brew install podman
podman machine init
podman machine start
```

### Windows

Download from [Podman Desktop](https://podman-desktop.io/) or use WSL2:

```bash
# In WSL2 Ubuntu
sudo apt-get install podman
```

That's it! No daemon to start, no groups to join. Just install and run.

> **Note for Docker users**: If you have existing Docker workflows, you can install `podman-docker` which provides a `docker` command alias that calls `podman`.

## How to use podmanrun.sh

Above all, this script must be executed at the root of the project, which is the equivalent of the `PVSNESLIB_HOME` environment variable.

No need to set this environment variable, the script takes care of it for you.

```
  ./podmanrun.sh -h
usage: ./podmanrun.sh options:<d|h|r|b>
-h: help, print this message
-d: followed by the name of the linux distribution (fedora, ubuntu or debian)
-r: Create release (make release)
-b: Run this script in batch mode, useful when using without terminal
```

### Use cases

I just want to compile my code for ubuntu distribution.

> Notes: podmanrun.sh *must* be launched at the root of the repository as it used the current working directory to set `PVSNESLIB_HOME`.

```bash
podman/scripts/podmanrun.sh -d ubuntu
```

> Notes: The `-d` option is the only mandatory option (except help `-h` menu).

I want to compile my code for my **ubuntu** distribution and create the release (`make release`).

```bash
podman/scripts/podmanrun.sh -d ubuntu -r
```

The release file is located in the `release` directory.

Finally I want to integrate this script in batch mode (without terminal), without progress bars to keep readable logs.

```bash
podman/scripts/podmanrun.sh -d ubuntu -b
```

## Remarks

The **centos** and **fedora** versions (especially) take longer on first use (to build the container image). Made some changes to improve build times.

## Todo

Create a Dockerfile for the latest **windows** and **macos** O.S.

## To contribute

Do not hesitate to give your feedback, suggest changes or simply bring some yourself.
