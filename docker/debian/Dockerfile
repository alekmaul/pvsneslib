FROM debian:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "
ENV DISTRO $DISTRO

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN apt-get update -y && \
    dpkg --add-architecture i386 && \
    apt-get install build-essential gcc-multilib git python-is-python3 cmake wget -y

CMD /bin/echo -ne "\n\t>> [MAKE]   Cleaning files                   " && \
    make clean > ${PVSNESLIB_HOME}/docker/${DISTRO}/make_clean.log 2>&1 && \
    /bin/echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1) && \
    /bin/echo -ne "\t>> [GIT]    Refreshing source code           " && \
    git submodule update --init > ${PVSNESLIB_HOME}/docker/${DISTRO}/git_refresh.log 2>&1 && \
    /bin/echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1) && \
    /bin/echo -ne "\t>> [MAKE]   Compiling code and installing it " && \
    make > ${PVSNESLIB_HOME}/docker/${DISTRO}/make.log 2>&1 && \
    /bin/echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1)

