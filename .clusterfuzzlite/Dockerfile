FROM gcr.io/oss-fuzz-base/base-builder:v1@sha256:cb8b7ff401eeb38a378428e6d9ec5a7dc18441ba999e1db21f1641d4debf8f3b

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
