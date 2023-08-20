# Build and release sources from docker container

This readme explains how and why to use docker containers.

- [Build and release sources from docker container](#build-and-release-sources-from-docker-container)
  - [Why use docker for this project](#why-use-docker-for-this-project)
  - [Features](#features)
  - [Integrated linux distributions](#integrated-linux-distributions)
  - [Installation](#installation)
  - [How to use dockerrun.sh](#how-to-use-dockerrunsh)
    - [Use cases](#use-cases)
  - [Remarks](#remarks)
  - [Todo](#todo)
  - [To contribute](#to-contribute)

## Why use docker for this project

- Each distribution uses different package versions, by testing the most used distributions, it is easier to trace the regressions between each release of **pvsneslib**.
- You don't want to install all the development packages on your OS but you want to be able to compile the code and test by yourself.
- The Dockerfiles serve as documentation if you want to test the compilation locally on your distribution but you don't know which packages to install.


## Features

- Compile **pvnseslib** tools from different linux distribution versions.
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

As a prerequisite, you must install docker-ce on your distribution.

* [centos](https://docs.docker.com/engine/install/centos/)
* [fedora](https://docs.docker.com/engine/install/fedora/)
* [debian](https://docs.docker.com/engine/install/debian/)
* [ubuntu](https://docs.docker.com/engine/install/ubuntu/)

These distributions have been integrated because they represent the most used ones (not exhaustive). [See Top Linux Subcategories by market share](https://truelist.co/blog/linux-statistics/).


Activate and start the docker service.

```bash
 sudo systemctl enable docker
 sudo systemctl start docker
```

Add your current user to the docker group and restart your system to make it effective.

```bash
  sudo usermod -aG docker ${USER}
```

## How to use dockerrun.sh

Above all, this script must be executed at the root of the project, which is the equivalent of the `PVSNESLIB_HOME` environment variable.

No need to set this environment variable, the script takes care of it for you.

```
  ./dockerrun -h
usage: ./dockerrun.sh options:<d|h|r|b>
-h: help, print this message
-d: followed by the name of the linux distribution (fedora, ubuntu or debian)
-r: Create release (make release)
-b: Run this script in batch mode, useful when using without terminal
```

### Use cases

I just want to compile my code for ubuntu distribution.

> Notes: dockerrun.sh *must* be launched at the root of the repository as it used the current working directory to set `PVSNESLIB_HOME`.

```bash
  docker/scripts/dockerrun.sh -d ubuntu

```

> Notes: The `-d` option is the only mandatory option (except help `-h` menu).

I want to compile my code for my **ubuntu** distribution and create the release (`make release`).

```bash
  docker/scripts/dockerrun.sh -d ubuntu -r

```

The release file is located in the `release` directory.

Finally I want to integrate this script in batch mode (without terminal), without progress bars to keep readable logs.

```bash
  ./dockerrun -d ubuntu -b

```

## Remarks

The **centos** and **fedora** versions (especially) take longer on first use (to build the docker image). Made some changes to improve build times.

## Todo

Create a Dockerfile for the latest **windows** and **macos** O.S.

## To contribute

Do not hesitate to give your feedback, suggest changes or simply bring some yourself.
