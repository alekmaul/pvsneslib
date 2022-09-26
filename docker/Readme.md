# Build code from docker container

This readme explains how and why to use docker containers.

- [Build code from docker container](#build-code-from-docker-container)
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
- Have the possibility to add continuous integration by testing the modifications with each modification
- Have the possibility to push each new version of pvsneslib in github in the release section
- And so on...

## Features

- Compile **pvnseslib** tools from different linux distribution versions.
- Create ready-to-use releases in zip form
- Check that pvsneslib compiles correctly on different linux distributions, and thus test different versions of tools and libraries used to compile.

> There is no relationship between the version of Linux you are using and the type of container used. The binaries do not use the OS libraries.

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
```
 sudo systemctl enable docker
 sudo systemctl start docker
```

Add your current user to the docker group and restart your system to make it effective.
```
  sudo usermod -aG docker <username>
```

## How to use dockerrun.sh

Above all, this script must be executed at the root of the project, which is the equivalent of `PVSNESLIB_HOME`.

No need to set this environment variable, the script takes care of it for you.

```
  ./dockerrun -h

usage: ./dockerrun.sh options:<d|h|c>
-d: name of the linux distrubtion (fedora, ubuntu or debian)
-h: help, print this message
-r: Create release (zip) to /var/tmp/build by default or by providing a custon path in parameter
-b: Run this script in batch mode, useful when using without terminal

```

### Use cases

I just want to compile my code for my ubuntu distribution.

> Notes: dockerrun.sh *must* be lauched at the root of the repository as it used the current working directory to set `PVSNESLIB_HOME`.

```
  docker/scripts/dockerrun.sh -d ubuntu

```
> Notes: The `-d` option is the only mandatory option (except help `-h` menu).

I want to compile my code for my **ubuntu** distribution and create my own release (zip).

```
  docker/scripts/dockerrun.sh -d ubuntu -r

```

In this case, the archive will be located in `/var/tmp`.

Ditto but I want to put my archive in a custom directory.

```
  docker/scripts/dockerrun.sh -d ubuntu -r /somewhere/over/the/rainbow

```

Finally I want to integrate this script in batch mode (without terminal), without progress bars to keep readable logs.

```
  ./dockerrun -d ubuntu -b

```

## Remarks

The **centos** and **fedora** versions (especially) take longer on first use (to build the docker image). Made some changes to improve build times.


## Todo

- Add the ability to specify a **git tag** and build a release from it.
- Add continuous integration to pre-validate each pull request.
  - In addition to validating the correct compilation of the code, we could add a checksum test on the sfc files in the examples, comparing them to those of the latest versions. If the hash does not change, everything is fine, otherwise raise a warning and test the rom manually. This avoids systematically testing each rom following a new version of **pvsneslib**.
- Add continuous delivery by creating pushing a new release in **github** automatically.

## To contribute

Do not hesitate to give your feedback, suggest changes or simply bring some yourself.
