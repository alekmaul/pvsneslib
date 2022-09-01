#!/bin/bash

function f_usage {

    echo "usage: ${0} options:<d|h|c>"
    echo "-d: name of the linux distrubtion (fedora, ubuntu or debian)"
    echo "-h: help, print this message"
    echo "-c: remove all containers <!> it will delete your running container(s) as well"

}

function f_check_distro {

    distro="${1}"

    if [[ $(echo ${distro} | egrep -c 'fedora|debian|ubuntu') -ne 1 ]]; then

        echo "Wrong distrution: ${distro}"

        f_usage
        exit 1
    fi

}

function f_delete_container {

    is_container=$(docker ps -q -a | wc -l)

    if [[ ${is_container} -ne 0 ]]; then

        echo ">> [DOCKER] cleaning ${is_container} containers(s)"

        docker rm $(docker ps -q -a)

        echo
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

        echo
    fi

}

# --------------- #
# M A I N
# --------------- #

clear

unset -v distro

no_args="true"

while getopts "d:hc" flag; do
    case "${flag}" in
    d)
        distro=${OPTARG}
        f_check_distro ${distro}
        ;;
    h)
        f_usage
        exit 0
        ;;
    c)
        f_delete_container
        exit 0
        ;;
    esac
    no_args="false"
done

[[ "$no_args" == "true" ]] && (
    f_usage
    exit 1
)

f_delete_none_image

image="pvsneslib-${distro}-image"

echo -n ">> [DOCKER] Building image ${image} "
docker build \
    -f docker/${distro}/Dockerfile \
    -t ${image} . >$(pwd)/docker/${distro}/docker_build.log 2>&1
echo -e "[PASS]\n" || (
    echo -e "[FAILED]\n"
    exit 1
)

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
