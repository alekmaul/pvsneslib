#!/bin/bash

# --------------- #
# M A I N
# --------------- #

source "podman/scripts/include/libraries.sh"

trap f_quit SIGINT
trap f_quit SIGTERM

clear

PVSNESLIB_HOME="$(pwd)"

no_args="true"
padding_width=50
batch_mode="false"
make_release="false"

while getopts ":d:rhb" flag; do
    case "${flag}" in
    d)
        distro=${OPTARG}
        f_check_distro
        release_file="pvsneslib-${distro}"
        image="${release_file}-image"
        ;;
    h)
        f_usage
        exit 0
        ;;
    r)
        make_release="true"
        ;;
    b)
        batch_mode="true"
        ;;
    *)
        f_usage
        exit 1
        ;;
    esac
    no_args="false"
done

export padding_width \
    release_path \
    make_release \
    batch_mode \
    PVSNESLIB_HOME \
    image

[[ "$no_args" == "true" ]] && {
    f_usage
    exit 1
}

f_build_podman_image

f_run_podman_container
