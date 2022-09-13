FROM debian:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "
ENV DISTRO $DISTRO

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN apt-get update -y && \
    dpkg --add-architecture i386 && \
    apt-get install -y build-essential \
        gcc-multilib \
        git \
        python-is-python3 \
        cmake \
        wget

CMD ./docker/scripts/container.sh "${PVSNESLIB_HOME}" "${DISTRO}"

