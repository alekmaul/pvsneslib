#!/bin/bash

# shellcheck disable=SC2154

function f_usage {

    echo "usage: ${0} options:<d|h|c>"
    echo "-d: name of the linux distrubtion (fedora, ubuntu or debian)"
    echo "-h: help, print this message"
    echo "-r: Create release (zip) to /var/tmp/build"
    echo "-b: Run this script in batch mode, useful when using without terminal"

}

function f_print_progress_bar {

    docker_build_pid=${1}
    sp='/-\|'
    printf ' '

    while kill -0 "${docker_build_pid}" 2>/dev/null; do
        printf '\r%s %s \b%.1s' "Preparing docker image" "  " "$sp"
        sp=${sp#?}${sp%???}
    done

    clear

}

function f_check_distro {

    if [[ $(echo "${distro}" | grep -Ec '^centos$|^fedora$|^debian$|^ubuntu$') -ne 1 ]]; then

        echo "Wrong distrution: ${distro}"

        f_usage
        exit 1
    fi

}

function f_build_docker_image {

    if [[ ${batch_mode} == "false" ]]; then
        docker build \
            -f "docker/${distro}/Dockerfile" \
            -t "${image}" . >"${PVSNESLIB_HOME}/docker/${distro}/docker_build.log" 2>&1 &
        my_pid=$!
        f_print_progress_bar ${my_pid}
        wait ${my_pid}
    else
        docker build \
            -f "docker/${distro}/Dockerfile" \
            -t "${image}" . >"${PVSNESLIB_HOME}/docker/${distro}/docker_build.log" 2>&1
    fi

    f_print_message "DOCKER" "Building image $image" $?

}

function f_run_docker_container {

    echo -e "[DOCKER] Starting Container ${distro} [...]\n"

    docker run -ti --rm \
        -w "${PVSNESLIB_HOME}" \
        -v "${PVSNESLIB_HOME}:${PVSNESLIB_HOME}" \
        -v "/etc/group:/etc/group:ro" \
        -v "/etc/passwd:/etc/passwd:ro" \
        -u "$(id -u):$(id -g)" \
        -e "PVSNESLIB_HOME=${PVSNESLIB_HOME}" \
        -e "DISTRO=${distro}" \
        "${image}"

    f_print_message "DOCKER" "Running container with tasks" $?

}

function f_create_zip {

    if [[ "${create_zip}" == "true" ]]; then
        {
            [[ -d "${release_path}/pvsneslib" ]] ||
                mkdir -p "${release_path}/pvsneslib" >/dev/null 2>&1
            cp -pvr pvsneslib snes-examples devkitsnes \
                "${release_path}/pvsneslib"
            cd "${release_path}" || exit 1
        } >/dev/null 2>&1

        zip -qr "${release_file}.zip" pvsneslib; exit_code=$?

        {
            rm -rf pvsneslib
            cd "${PVSNESLIB_HOME}" || exit 1
        } >/dev/null 2>&1

        f_print_message "ZIP" "Building the release file ${release_file}" ${exit_code}

        echo -e "\tThe release file is ${release_path}/${release_file}.zip"
    fi
}

function f_print_os_info {

    echo ">>>> SYSTEM INFORMATION" >"${PVSNESLIB_HOME}/docker/${distro}/infos.log"

    {
        echo -n "O.S version "
        grep -E '^PRETTY_NAME' /etc/os-release |
            cut -d "=" -f2 |
            tr -d "\""
        echo -n "Kernel version "
        uname -a
        docker -v
    } >>"${PVSNESLIB_HOME}/docker/${distro}/infos.log"
    echo

}

function f_print_message {

    #width=50

    action="${1}"
    message="${2}"
    status=${3}

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

function f_check_sfc_md5 {

    mapfile -t sfc_md5hash < <(cat "${PVSNESLIB_HOME}/docker/md5/sfc_md5sum.ref")

    sfc_missing=0
    declare -a wrong_sfc_md5=()

    for line in "${sfc_md5hash[@]}"; do
            sfc_hash="$(echo "${line}" | cut -d "|" -f1)"
            sfc_file="$(echo "${line}" | cut -d "|" -f2)"

            if [[ ! -f ${sfc_file} ]]; then
               echo "<!!> ${sfc_file} is missing"
                sfc_missing=1
                break

            fi

            if [[ "$(md5sum "${PVSNESLIB_HOME}/${sfc_file}" | cut -d " " -f1)" != "${sfc_hash}" ]]; then
                    {
                        wrong_sfc_md5+=("${sfc_file}")
                    }
            fi
    done

    if [[ ${sfc_missing} -eq 1 ]]; then
        f_print_message "SFC" "Checking SFC file(s)" 1
    else
        f_print_message "SFC" "Checking SFC file(s)" 0
        if [[ ${#wrong_sfc_md5[@]} -ne 0 ]]; then
            echo "<!> ${#wrong_sfc_md5[@]} SFC rom(s) differ, please check it with an emulator:"
            for sfc in "${wrong_sfc_md5[@]}"; do
                echo "  - ${sfc}"
            done
            echo

        fi
    fi

}
