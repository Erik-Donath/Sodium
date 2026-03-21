#!/usr/bin/env bash
set -e

ARCH=${1:-i686}
BUILD_TYPE=${2:-Debug}

echo "--> Configuring for arch=${ARCH} build=${BUILD_TYPE}"

cmake -S /root/env/buildsystem/cmake/${ARCH} \
      -B /root/env/build \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DSRC=/root/env/src

echo "--> Building"
cmake --build /root/env/build --target sodium.iso

echo "--> Done"
