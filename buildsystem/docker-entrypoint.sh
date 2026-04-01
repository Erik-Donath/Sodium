#!/usr/bin/env bash
set -e

ARCH=${1:-i686}
BUILD_TYPE=${2:-Debug}

echo "--> Configuring for arch=${ARCH} build=${BUILD_TYPE}"

# Delete the CMake cache before every configure run.
# This ensures execute_process() results (like the libgcc path) are never
# stale, while compiled object files are still kept for incremental builds.
rm -f /workspace/build/CMakeCache.txt

cmake -S /workspace/buildsystem/cmake/${ARCH} \
      -B /workspace/build \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DSRC=/workspace/src

echo "--> Building"
cmake --build /workspace/build --target sodium.iso

echo "--> Done"