#!/bin/bash -e

clear

echo -e "\n>> [DOCKER] Building image"

docker build -q -t pvsneslib-image .

echo -e "\n>> [DOCKER] Running container"

docker run -ti --rm \
    -w "$(pwd)" \
    -v "$(pwd):$(pwd)" \
    -u "$(id -u):$(id -g)" \
    -e "PVSNESLIB_HOME=$(pwd)" \
    pvsneslib-image
