FROM fedora:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN echo "minrate=30k" >> /etc/dnf/dnf.conf
RUN echo "max_parallel_downloads=10" >> /etc/dnf/dnf.conf
RUN echo "fastestmirror=True" >> /etc/dnf/dnf.conf

RUN dnf upgrade --refresh -y && \
dnf -y groupinstall "Development Tools" && \
dnf -y install glibc-devel.i686 cmake gcc-c++ python

CMD echo && \
    echo -n ">> [MAKE]   Cleaning files                 " && \
    make clean > ./make_clean.log 2>&1 && \
    echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1) && \
    echo -n ">> [GIT]    Refreshing source code         " && \
    git submodule update --init && > git_refresh.log 2>&1 && \
    echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1) && \
    echo -n ">> [MAKE]   Compiling code and install it  " && \
    make > make.log 2>&1 && \
    echo -e "[PASS]\n" || (echo -e "[FAILED]\n"; exit 1)
