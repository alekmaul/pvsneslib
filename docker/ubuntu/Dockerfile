FROM ubuntu:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV DISTRO $DISTRO

RUN apt-get update -y && \
    dpkg --add-architecture i386 && \
    apt-get install build-essential gcc-multilib git python-is-python3 cmake wget -y

CMD /usr/bin/echo -ne "\n\t>> [MAKE]   Cleaning files                   " && \
    make clean > ${PVSNESLIB_HOME}/docker/${DISTRO}/make_clean.log 2>&1 && \
    /usr/bin/echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1 } && \
    /usr/bin/echo -ne "\t>> [GIT]    Refreshing source code           " && \
    git submodule update --init > ${PVSNESLIB_HOME}/docker/${DISTRO}/git_refresh.log 2>&1 && \
    /usr/bin/echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1 } && \
    /usr/bin/echo -ne "\t>> [MAKE]   Compiling code and installing it " && \
    make > ${PVSNESLIB_HOME}/docker/${DISTRO}/make.log 2>&1 && \
    /usr/bin/echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1 }
