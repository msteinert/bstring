FROM gcr.io/oss-fuzz-base/base-builder@sha256:2322953510439e9e74a031bfcb6b2025cff75e5d4c16d4706470de8383940d45

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
