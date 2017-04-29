FROM mayfieldrobotics/ubuntu:14.04

ENV DEBIAN_FRONTEND="noninteractive" \
    TERM="xterm"

ARG PKG_NAME
ARG SRC_VERSION
ARG PKG_RELEASE
ARG ARTIFACTS_DIR

ENV INSTALL_DIR="/tmp/installdir"
ENV PKG_VERSION="${SRC_VERSION}-${PKG_RELEASE}"

RUN apt-get update -qq \
  && apt-get install -yq \
    autoconf \
    automake \
    build-essential \
    iptables-dev \
    libdbus-1-dev \
    libglib2.0-dev \
    libgnutls-dev \
    libreadline-dev \
    libtool \
    openconnect \
    openvpn \
    pkg-config \
    ruby-dev \
    vpnc \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

RUN gem install --no-ri --no-rdoc fpm

COPY . /root/connman/

WORKDIR /root/connman

RUN ./bootstrap-configure-mf \
  && make \
  && mkdir -p ${INSTALL_DIR} \
  && make install DESTDIR=${INSTALL_DIR}

RUN install --mode 0644 -D src/connman-upstart.conf \
      ${INSTALL_DIR}/etc/init/connman.conf

# NOTE: The dependencies below were copied from connman 1.15-0ubuntu3 (Trusty)
#       Actual dependencies of 1.33: http://packages.ubuntu.com/xenial/connman
RUN fpm \
  --input-type dir \
  --chdir ${INSTALL_DIR} \
  --output-type deb \
  --architecture native \
  --name ${PKG_NAME} \
  --version ${PKG_VERSION} \
  --description "Intel's Connection Manager daemon patched by Mayfield." \
  --depends "libc6 (>= 2.15), libdbus-1-3 (>= 1.1.1), \
             libglib2.0-0 (>= 2.28.0), libgnutls26 (>= 2.12.17-0), \
             libreadline6 (>= 6.0), libxtables10, iptables, dbus, lsb-base, \
             bluez, wpasupplicant" \
  --conflicts "connman, network-manager, wicd" \
  --provides "connman" \
  --replaces "connman" \
  --deb-recommends "bluez, wpasupplicant" \
  --license "GPL v2.0" \
  --vendor "Mayfield Robotics" \
  --maintainer "Spyros Maniatopoulos <spyros@mayfieldrobotics.com>" \
  --url "https://github.com/mayfieldrobotics/connman" \
  .

RUN mkdir -p ${ARTIFACTS_DIR} \
  && mv *.deb ${ARTIFACTS_DIR}
