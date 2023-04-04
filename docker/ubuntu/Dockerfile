FROM ubuntu:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV DISTRO $DISTRO

RUN apt-get update -y && \
    dpkg --add-architecture i386 && \
    apt-get install -y build-essential \
        gcc-multilib \
        cmake \
        wget

CMD ./docker/scripts/container.sh "${PVSNESLIB_HOME}" "${DISTRO}"
