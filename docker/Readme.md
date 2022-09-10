
# Build code from docker container

- [Build code from docker container](#build-code-from-docker-container)
  - [Features](#features)
  - [Installation](#installation)
  - [Todo](#todo)

This page explains how to build your code via a linux container.


## Features

- Build code from fedora, debian and ubuntu containers
- Create release (zip file) in /var/tmp/build


## Installation

Install docker-ce according to your distribution

* [fedora](https://docs.docker.com/engine/install/fedora/)
* [debian](https://docs.docker.com/engine/install/debian/)
* [ubuntu](https://docs.docker.com/engine/install/ubuntu/)
* [centos](https://docs.docker.com/engine/install/centos/)


Activate and start the docker service
```
 sudo systemctl enable docker
 sudo systemctl start docker
```

Add your current user to the docker group and restart your system to make it effective
```
  sudo usermod -aG docker <username>
```

Then, execute the dockerrrun.sh script. Four options are availables
* -h: see help
```
  ./dockerrrun -h

usage: ./dockerrun.sh options:<d|h|c>
-d: name of the linux distrubtion (fedora, ubuntu or debian)
-h: help, print this message
-r: Create release (zip) to /var/tmp/build by default or by providing a custon path in parameter
-b: Run this script in batch mode, useful when using without terminal

```
* -d: specify the distribution (mandatory). Fedora, debian and ubuntu are availables (latest version)
```
  ./dockerrrun -d fedora

>> [DOCKER] Building image pvsneslib-fedora-image [PASS]

>> [DOCKER] Running container pvsneslib-fedora-image...

        >> [MAKE]   Cleaning files                   [PASS]

        >> [GIT]    Refreshing source code           [PASS]

        >> [MAKE]   Compiling code and installing it [PASS]

```
* -r: create a zip file (release) for the current build (so fedora, ubuntu or debian). Must be used with `-d <distribution>`. Here an output with ubuntu container:
```
    ./dockerrrun -d fedora -r

>> [DOCKER] Building image pvsneslib-ubuntu-image [PASS]

>> [DOCKER] Running container pvsneslib-ubuntu-image...

        >> [MAKE]   Cleaning files                   [PASS]

        >> [GIT]    Refreshing source code           [PASS]

        >> [MAKE]   Compiling code and installing it [PASS]


>> [ZIP] Building release for pvsneslib-ubuntu-image [PASS]

The release is /var/tmp/build/pvsneslib-ubuntu-image.zip
```
You can ever build all distrutions et create release (zip) files on the fly as follows
```bash
$ for d in fedora debian ubuntu; do
    ./dockerrun.sh -d $d -r
done
```

This script doesn't print any output from the executed tasks.
Each tasks are stored in a dedicated logfile in `docker/<distribution>/`

## Todo

- Add tag version when creating the release (zip)
- Add CI: launch build and tests on each PR to be validated
- Add CD: push release on github when all steps are successful