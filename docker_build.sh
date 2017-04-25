#!/usr/bin/env sh

set -eux

# Build a Docker image that compiles and packages ConnMan
DOCKER_IMAGE="connman-build"
DOCKER_ARTIFACTS="/root/artifacts"

docker build \
  --build-arg CONNMAN_VER="1.33" \
  --build-arg PKG_RELEASE="0" \
  --build-arg MAYFIELD_VER="0" \
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
dpkg-deb --info ${LOCAL_ARTIFACTS}/connman_*
dpkg-deb --contents ${LOCAL_ARTIFACTS}/connman_*
