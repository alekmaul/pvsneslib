FROM centos:latest
LABEL maintainer="kobenairb <kobenairb@gmail.com>"

ENV TERM xterm
ENV PVSNESLIB_HOME $(pwd)
ENV PS1A="[\d \[\e[33m\]\t\[\e[m\]] \[\e[31m\]\u\[\e[m\]@\[\e[31m\]\h\[\e[m\] \[\e[36m\]\w\[\e[m\]\n $ "
ENV DISTRO $DISTRO

RUN echo 'PS1=$PS1A' >> ~/.bashrc

RUN sed -i 's/mirrorlist/#mirrorlist/g' \
        /etc/yum.repos.d/CentOS-*
RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' \
        /etc/yum.repos.d/CentOS-*

RUN echo "minrate=30k" >> /etc/dnf/dnf.conf

RUN echo "max_parallel_downloads=10" >> /etc/dnf/dnf.conf

RUN dnf -y upgrade && \
    dnf -y groupinstall "Development Tools" && \
    dnf -y install dnf-plugins-core && \
    dnf -y config-manager --set-enabled powertools && \
    dnf -y install glibc-devel.i686 \
        cmake\
        gcc-c++ \
        glibc-static \
        libstdc++-static && \
    alternatives --set python /usr/bin/python3

CMD ./docker/scripts/container.sh "${PVSNESLIB_HOME}" "${DISTRO}"
