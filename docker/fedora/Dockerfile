FROM fedora:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "
ENV DISTRO $DISTRO

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN echo "minrate=30k" >> /etc/dnf/dnf.conf
RUN echo "max_parallel_downloads=10" >> /etc/dnf/dnf.conf
RUN echo "fastestmirror=True" >> /etc/dnf/dnf.conf

RUN dnf upgrade --refresh -y && \
    dnf -y groupinstall "Development Tools" && \
    dnf -y install glibc-devel.i686 cmake gcc-c++ python glibc-static libstdc++-static

CMD echo -e "\n>>>>> CONTAINER INFORMATIONS" >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    echo -n "O.S version " >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    egrep '^PRETTY_NAME' /etc/os-release | \
        cut -d "=" -f2 |\
        sed 's/"//g' >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    echo -n "Kernel version " >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    uname -a >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    echo "gcc version:" >> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    gcc -v 2>> ${PVSNESLIB_HOME}/docker/${DISTRO}/infos.log && \
    echo -ne "\n\t>> [MAKE]   Cleaning files                   " && \
    make clean > ${PVSNESLIB_HOME}/docker/${DISTRO}/make_clean.log 2>&1 && \
    echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1; } && \
    echo -ne "\t>> [GIT]    Refreshing source code           " && \
    git submodule update --init > ${PVSNESLIB_HOME}/docker/${DISTRO}/git_refresh.log 2>&1 && \
    echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1; } && \
    echo -ne "\t>> [MAKE]   Compiling code and installing it " && \
    make > ${PVSNESLIB_HOME}/docker/${DISTRO}/make.log 2>&1 && \
    echo -e "[PASS]\n" || { echo -e "[FAILED]\n"; exit 1; }
