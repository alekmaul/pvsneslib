#!/bin/bash

source "./docker/scripts/include/libraries.sh"

PVSNESLIB_HOME="${1}"
DISTRO="${2}"
padding_width=35

export padding_width

git fetch --all >"${PVSNESLIB_HOME}/docker/${DISTRO}/git_fetch_all.log" 2>&1
f_print_message "GIT" "Feching all" $?

git submodule update --init >"${PVSNESLIB_HOME}/docker/${DISTRO}/git_update_submodule.log" 2>&1
f_print_message "GIT" "Updating submodules" $?

make clean >"${PVSNESLIB_HOME}/docker/${DISTRO}/make_clean.log" 2>&1
f_print_message "MAKE" "Cleaning" $?

make >"${PVSNESLIB_HOME}/docker/${DISTRO}/make.log" 2>&1
f_print_message "MAKE" "Compiling and installing" $?
