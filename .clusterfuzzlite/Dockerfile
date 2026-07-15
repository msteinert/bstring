FROM gcr.io/oss-fuzz-base/base-builder@sha256:b38b34172cfa570f6c2b87ce6d7204843c1d4c3cd2cb7a5c83d6bb1a8ea93be5

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
