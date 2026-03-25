FROM gcr.io/oss-fuzz-base/base-builder:v1@sha256:9caf963eedfdcd5363da61e08943baace25ceb1e4ecd0fe8b395910e88d40573

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
