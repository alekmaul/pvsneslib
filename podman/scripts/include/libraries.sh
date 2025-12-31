#!/bin/bash

function f_usage {

    echo "usage: ${0} options:<d|h|r|b>"
    echo "-h: help, print this message"
    echo "-d: followed by the name of the linux distribution (fedora, ubuntu or debian)"
    echo "-r: Create release (make release)"
    echo "-b: Run this script in batch mode, useful when using without terminal"

}

function f_print_progress_bar {

    podman_build_pid=${1}
    sp='/-\|'
    printf ' '

    while kill -0 "${podman_build_pid}" 2>/dev/null; do
        printf '\r%s %s \b%.1s' "Preparing container image" "  " "$sp"
        sp=${sp#?}${sp%???}
    done

    clear

}

function f_check_distro {

    if [[ $(echo "${distro}" | grep -Ec '^centos$|^fedora$|^debian$|^ubuntu$') -ne 1 ]]; then

        echo "Wrong distrution: ${distro}, supported distributions are centos, fedora, ubuntu or debian"

        f_usage
        exit 1
    fi

}

function f_build_podman_image {

    if [[ ${batch_mode} == "false" ]]; then
        podman build \
            -f "podman/${distro}/Dockerfile" \
            -t "${image}" . >"${PVSNESLIB_HOME}/podman/${distro}/podman_build.log" 2>&1 &
        my_pid=$!
        f_print_progress_bar ${my_pid}
        wait ${my_pid}
    else
        podman build \
            -f "podman/${distro}/Dockerfile" \
            -t "${image}" . >"${PVSNESLIB_HOME}/podman/${distro}/podman_build.log" 2>&1
    fi

    f_print_message "PODMAN" "Building image $image" $?

}

function f_run_podman_container {

    echo -e "[PODMAN] Starting Container ${distro} [...]\n"

    podman run -ti --rm \
        -w "${PVSNESLIB_HOME}" \
        -v "${PVSNESLIB_HOME}:${PVSNESLIB_HOME}" \
        -e "PVSNESLIB_HOME=${PVSNESLIB_HOME}" \
        -e "DISTRO=${distro}" \
        -e "MAKE_RELEASE=${make_release}" \
        "${image}"

    f_print_message "PODMAN" "Running container with tasks" $?

}

function f_print_message {

    #width=50

    action="${1}"
    message="${2}"
    status="${3}"

    if [[ $status -eq 0 ]]; then
        fmt_status="PASS"
    else
        fmt_status="FAIL"
    fi

    printf ">>> [%s] %-*s[%s]\n\n" "$action" "$((padding_width - $((${#3} + ${#1}))))" "$message" "$fmt_status"

    [[ $status -eq 0 ]] || exit "$status"

}

function f_quit {

    echo "Do you want to quit ? (y/n)"
    read -re ctrlc
    if [ "$ctrlc" = 'y' ]; then
        exit 1
    fi

}
