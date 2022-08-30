#!/bin/bash

docker build -t pvsneslib-image .

docker run -ti --rm \
    -w "$(pwd)" \
    -v "$(pwd):$(pwd)" \
    -u "$(id -u):$(id -g)" \
    -e "PVSNESLIB_HOME=$(pwd)" \
    pvsneslib-image