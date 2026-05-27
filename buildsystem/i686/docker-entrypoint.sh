#!/usr/bin/env bash
set -e

BUILD_TYPE=${1:-Debug}

echo "--> Configuring for arch=i686 build=${BUILD_TYPE}"

cmake -S /workspace/buildsystem/i686 \
      -B /workspace/build \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DSRC=/workspace/src

echo "--> Building"
cmake --build /workspace/build --target sodium.iso

echo "--> Done"