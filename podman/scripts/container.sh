#!/bin/bash

source "podman/scripts/include/libraries.sh"

PVSNESLIB_HOME="${1}"
DISTRO="${2}"
MAKE_RELEASE="${3}"
padding_width=35

export padding_width

git fetch --all >"${PVSNESLIB_HOME}/podman/${DISTRO}/git_fetch_all.log" 2>&1
f_print_message "GIT" "Fetching all" $?

git submodule update --init >"${PVSNESLIB_HOME}/podman/${DISTRO}/git_update_submodule.log" 2>&1
f_print_message "GIT" "Updating submodule(s)" $?

make clean >"${PVSNESLIB_HOME}/podman/${DISTRO}/make_clean.log" 2>&1
f_print_message "MAKE" "Cleaning" $?

if [[ ${MAKE_RELEASE} == "false" ]]; then
    make >"${PVSNESLIB_HOME}/podman/${DISTRO}/make.log" 2>&1
    f_print_message "MAKE" "Compiling" $?
else
    make release >"${PVSNESLIB_HOME}/podman/${DISTRO}/make.log" 2>&1
    f_print_message "MAKE" "Compiling and releasing" $?
fi
