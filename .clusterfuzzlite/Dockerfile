FROM gcr.io/oss-fuzz-base/base-builder@sha256:93a6e18910480f02300374f68ded71bf8038f18aeb71e975265354e26b168bec

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
