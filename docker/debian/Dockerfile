FROM debian:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "
ENV DISTRO $DISTRO

ARG DEBIAN_FRONTEND=noninteractive

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN echo 'deb http://deb.debian.org/debian testing main' >> /etc/apt/sources.list

RUN apt-get update -y

RUN apt-get install -y tzdata

RUN apt-get upgrade -y

RUN apt-get install -y build-essential \
        gcc-12 \
        cmake \
        make \
        git \
        doxygen \
        texi2html \
        texlive-latex-base \
        texlive-latex-recommended \
        texlive-latex-extra

RUN unlink /etc/localtime

RUN ln -s /usr/share/zoneinfo/America/New_York /etc/localtime

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 90

CMD ./docker/scripts/container.sh "${PVSNESLIB_HOME}" "${DISTRO}"

