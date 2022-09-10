#!/bin/bash

# --------------- #
# M A I N
# --------------- #

source "./docker/scripts/include/libraries.sh"

trap f_quit SIGINT
trap f_quit SIGTERM

clear

no_args="true"
create_zip="false"
pvsneslib_home="$(pwd)"
padding_width=50
batch_mode="false"

while getopts ":d:r:hb" flag; do
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
        release_path=${OPTARG:=/var/tmp/build}
        create_zip="true"
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
    create_zip \
    batch_mode \
    pvsneslib_home \
    image

[[ "$no_args" == "true" ]] && {
    f_usage
    exit 1
}

f_print_os_info

f_build_docker_image

f_run_docker_container

f_create_zip
