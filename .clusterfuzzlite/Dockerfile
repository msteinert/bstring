FROM gcr.io/oss-fuzz-base/base-builder:v1@sha256:6fce2f40942176a0bb0c2cd6ee4bf5faea55e64ef3bee65f1c647cc2c8dcbe82
USER root
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        meson \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

COPY bstring/ $SRC/project/bstring/
COPY fuzz/ $SRC/project/fuzz/
COPY meson_options.txt $SRC/project/
COPY meson.build $SRC/project/
WORKDIR $SRC/project
COPY .clusterfuzzlite/build.sh $SRC/build.sh
