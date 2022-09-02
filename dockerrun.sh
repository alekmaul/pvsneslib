#!/bin/bash

function f_usage {

    echo "usage: ${0} options:<d|h|c>"
    echo "-d: name of the linux distrubtion (fedora, ubuntu or debian)"
    echo "-h: help, print this message"
    echo "-r: Create release (zip) to /var/tmp/build"

}

function f_check_distro {

    distro="${1}"

    if [[ $(echo ${distro} | egrep -c 'fedora|debian|ubuntu') -ne 1 ]]; then

        echo "Wrong distrution: ${distro}"

        f_usage
        exit 1
    fi

}

function f_delete_none_image {

    is_none=$(docker image ls | grep -c none)

    if [[ ${is_none} -ne 0 ]]; then

        echo ">> [DOCKER] cleaning ${is_none} <none> image(s)"

        docker image ls |
            grep none |
            awk '{print $3}' |
            xargs docker image rm --force >/dev/null 2>&1
    fi

}

function f_create_zip {

    image="${1}"
    rc=0
    realease_path="/var/tmp/build"

    echo -en "\n>> [ZIP] Building release for ${image} "
    mkdir -p ${realease_path}/pvsneslib >/dev/null 2>&1
    cp -pvr pvsneslib snes-examples devkitsnes \
        ${realease_path}/pvsneslib >/dev/null 2>&1
    cd ${realease_path} >/dev/null 2>&1
    zip -r ./${image}.zip \
        pvsneslib >/dev/null 2>&1
    rc=$?
    rm -rf pvsneslib
    cd - >/dev/null 2>&1
    if [[ ${rc} -eq 0 ]]; then
        echo -e "[PASS]\n"
    else
        echo -e "[FAILED]\n"
        exit ${rc}
    fi
    echo -e "The release is ${realease_path}/${image}.zip"
}

function f_print_os_info {

    echo ">>>> SYSTEM INFORMATION" >$(pwd)/docker/${distro}/infos.log
    echo -n "O.S version " >>$(pwd)/docker/${distro}/infos.log
    egrep '^PRETTY_NAME' /etc/os-release |
        cut -d "=" -f2 |
        sed 's/"//g' >>$(pwd)/docker/${distro}/infos.log
    echo -n "Kernel version " >>$(pwd)/docker/${distro}/infos.log
    uname -a >>$(pwd)/docker/${distro}/infos.log
    docker -v >>$(pwd)/docker/${distro}/infos.log
    echo

}

function f_quit {

    echo "Do you want to quit ? (y/n)"
    read -e ctrlc
    if [ "$ctrlc" = 'y' ]; then
        exit
    fi

}

# --------------- #
# M A I N
# --------------- #

trap f_quit SIGINT
trap f_quit SIGTERM

clear

unset -v distro

no_args="true"
create_zip="false"

while getopts "d:rh" flag; do
    case "${flag}" in
    d)
        distro=${OPTARG}
        f_check_distro ${distro}
        ;;
    h)
        f_usage
        exit 0
        ;;
    r)
        create_zip="true"
        ;;
    esac
    no_args="false"
done

[[ "$no_args" == "true" ]] && {
    f_usage
    exit 1
}

f_delete_none_image

f_print_os_info

image="pvsneslib-${distro}-image"

echo -n ">> [DOCKER] Building image ${image} "
docker build \
    -f docker/${distro}/Dockerfile \
    -t ${image} . >$(pwd)/docker/${distro}/docker_build.log 2>&1
echo -e "[PASS]\n" || {
    echo -e "[FAILED]\n"
    exit 1
}

echo ">> [DOCKER] Running container ${image}..."
docker run -ti --rm \
    -w "$(pwd)" \
    -v "$(pwd):$(pwd)" \
    -v "/etc/group:/etc/group:ro" \
    -v "/etc/passwd:/etc/passwd:ro" \
    -u "$(id -u):$(id -g)" \
    -e "PVSNESLIB_HOME=$(pwd)" \
    -e "DISTRO=${distro}" \
    ${image}

[[ "$create_zip" == "true" ]] && f_create_zip ${image}
