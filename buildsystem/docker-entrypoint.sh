#!/usr/bin/env bash
set -e

ARCH=${1:-i686}
BUILD_TYPE=${2:-Debug}

echo "--> Configuring for arch=${ARCH} build=${BUILD_TYPE}"

cmake -S /workspace/buildsystem/cmake/${ARCH} \
      -B /workspace/build \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DSRC=/workspace/src

echo "--> Building"
cmake --build /workspace/build --target sodium.iso

echo "--> Done"