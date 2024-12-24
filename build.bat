docker build buildenv -t sodium-buildenv
docker run --rm -it -v %cd%:/root/env sodium-buildenv