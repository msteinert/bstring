#!/bin/bash -eu
#
# ClusterFuzzLite build script for bstring.
#
# ClusterFuzzLite sets:
#   $CC / $CXX     - clang
#   $CFLAGS        - sanitizer + coverage flags (e.g. -fsanitize=address,fuzzer-no-link)
#   $LDFLAGS       - sanitizer link flags
#   $LIB_FUZZING_ENGINE - fuzzer driver (e.g. -fsanitize=fuzzer)
#   $OUT           - output directory for fuzz target binaries
#
# Meson picks up $CC/$CFLAGS/$LDFLAGS from the environment during setup.
# Do not append $LIB_FUZZING_ENGINE to global LDFLAGS here: Meson's
# compiler sanity check links a regular main() and fails when libFuzzer's
# main() is injected globally.

PROJECT_SRC="$SRC/project"
if [[ ! -f "$PROJECT_SRC/meson.build" ]]; then
    PROJECT_SRC="$SRC/bstring"
fi

build_targets() {
    cd "$PROJECT_SRC"

    rm -rf build

    meson_args=(
        -Ddefault_library=static
        -Denable-docs=false
        -Denable-fuzzing=true
        -Denable-utf8=true
        -Denable-tests=false
        --buildtype=plain
    )

    if [[ -n "${LIB_FUZZING_ENGINE:-}" ]]; then
        meson_args+=("-Dfuzz-link-arg=${LIB_FUZZING_ENGINE}")
    fi

    meson setup build "${meson_args[@]}"
    ninja -C build
}

package_outputs() {
    cd "$PROJECT_SRC"
    cp build/fuzz/fuzz_bstring "$OUT/"
    zip -j "$OUT/fuzz_bstring_seed_corpus.zip" fuzz/corpus/*
}

build_targets
package_outputs
