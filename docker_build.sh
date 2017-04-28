#!/usr/bin/env sh

set -eux

# Debian package metadata
PKG_NAME="connman-mf"
SRC_VERSION="1.33"
PKG_RELEASE="0"

# Build a Docker image that compiles and packages ConnMan
DOCKER_IMAGE="connman-build"
DOCKER_ARTIFACTS="/root/artifacts"

docker build \
  --build-arg PKG_NAME=${PKG_NAME} \
  --build-arg SRC_VERSION=${SRC_VERSION} \
  --build-arg PKG_RELEASE=${PKG_RELEASE} \
  --build-arg ARTIFACTS_DIR=${DOCKER_ARTIFACTS} \
  --tag ${DOCKER_IMAGE} \
  .

# Extract the Debian packages from the Docker image
DOCKER_CONTAINER="connman-artifacts"
LOCAL_ARTIFACTS=$(basename ${DOCKER_ARTIFACTS})

mkdir -p ${LOCAL_ARTIFACTS}
rm -f ${LOCAL_ARTIFACTS}/*.deb

docker create --name ${DOCKER_CONTAINER} ${DOCKER_IMAGE}
docker cp "${DOCKER_CONTAINER}:${DOCKER_ARTIFACTS}" "./"
docker stop ${DOCKER_CONTAINER} && docker rm ${DOCKER_CONTAINER}

# Inspect the Debian packages
dpkg-deb --info ${LOCAL_ARTIFACTS}/${PKG_NAME}_*
dpkg-deb --contents ${LOCAL_ARTIFACTS}/${PKG_NAME}_*
