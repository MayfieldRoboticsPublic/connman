#!/usr/bin/env sh

set -eux

DOCKER_IMAGE="build-connman"

# Build the docker image
docker build \
  --tag ${DOCKER_IMAGE} \
  .
