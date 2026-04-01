# Compute the repo root relative to this Makefile's location so that
# docker -v mounts and cmake paths are always correct regardless of where
# make is invoked from.
SELF  := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
ROOT  := $(abspath $(SELF)/../..)

IMAGE := sodium-buildenv-i686
STAMP := $(SELF).docker-stamp
BUILD ?= Debug

.PHONY: build rebuild-image clean

$(STAMP): $(SELF)Dockerfile
	docker build $(SELF) -t $(IMAGE)
	docker inspect --format='{{.Id}}' $(IMAGE) > $(STAMP)

build: $(STAMP)
	docker run --rm \
	    --user $(shell id -u):$(shell id -g) \
	    -v $(ROOT):/workspace \
	    $(IMAGE) $(BUILD)

rebuild-image:
	docker build --no-cache $(SELF) -t $(IMAGE)
	docker inspect --format='{{.Id}}' $(IMAGE) > $(STAMP)

clean:
	rm -f $(STAMP)