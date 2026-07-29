FROM gcr.io/oss-fuzz-base/base-builder@sha256:d82c528a87611739621b2e243a6c6a0995a6635e4da446983fcc8a59555b94b8

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
