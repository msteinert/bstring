FROM gcr.io/oss-fuzz-base/base-builder@sha256:e8214e916e787f6109ef49983d97a72ea9062e1b26b7fe1d4661d1d2ff5fe276

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
