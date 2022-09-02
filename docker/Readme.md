
# Build code from docker container


This page explains how to build your code via a linux container.


## Features

- Build code from fedora, debian and ubuntu containers
- Create release (zip file) in /var/tmp/build


## Installation

Install docker on your linux system

Here an exemple for fedora systems
```bash
  sudo dnf -y install docker
  sudo systemctl enable docker.service
  sudo systemctl start docker.service
```

Add your current user to the docker group and restart your system to make it effective
```bash
  sudo usermod -aG docker <username>
```
## Usage/Examples

Clone the pvsneslib repository and move to the root of the project (pvsneslib)
```bash
  git clone git clone https://github.com/alekmaul/pvsneslib.git
  cd pvsneslib
```

Then, execute the dockerrrun.sh script. Two options are availables
* -h: see help
```bash
  ./dockerrrun -h

usage: ./dockerrun.sh options:<d|h|c>
-d: name of the linux distrubtion (fedora, ubuntu or debian)
-h: help, print this message
-r: Create release (zip) to /var/tmp/build
```
* -d: specify the distribution (mandatory). Fedora, debian and ubuntu are availables (latest version)
```bash
  ./dockerrrun -d fedora

>> [DOCKER] Building image pvsneslib-fedora-image [PASS]

>> [DOCKER] Running container pvsneslib-fedora-image...

        >> [MAKE]   Cleaning files                   [PASS]

        >> [GIT]    Refreshing source code           [PASS]

        >> [MAKE]   Compiling code and installing it [PASS]

```

This script doesn't print any output from the executed tasks.
Each tasks are stored as logfile in `docker/<distribution>/`, in this order:

1. docker_build.log: output of `docker build ...`
2. make_clean.log: output of `make Cleaning`
3. git_refresh.log: output of `git submodule update --init`
4. make.log: output of `make` and `make install`


## TODO

- Add tag version when creating the release (zip)
- Add CI: launch build and tests on each PR to be validated
- Add CD: push release on github when all steps are successful