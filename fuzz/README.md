# Fuzz Testing

A single libFuzzer fuzz target is provided which tests `bstrlib`, `bstraux`, and optionally `utf8util` and `buniutil`.

Unicode support is conditional: when the project is built with `-Denable-utf8=true`,
the target is compiled with `-DBSTRING_ENABLE_UTF8` and the unicode fuzzing code
is included automatically.

## Prerequisites

libFuzzer is part of LLVM.  The compiler runtime libraries are shipped in a
separate package from the compiler itself.

**Debian/Ubuntu:**

```sh
sudo apt-get install clang libclang-rt-dev
```

**Fedora:**

```sh
sudo dnf install clang compiler-rt
```

## Build

```sh
CC=clang meson setup build-fuzz \
    -Denable-fuzzing=true \
    -Denable-tests=false \
    --buildtype=plain
meson compile -C build-fuzz
```

## Run

```sh
# Run against the seed corpus for 60 seconds
./build-fuzz/fuzz/fuzz_bstring fuzz/corpus/ -max_total_time=60 -max_len=260

# Run indefinitely, saving new corpus entries and crash inputs as they are found
mkdir -p fuzz/crashes
./build-fuzz/fuzz/fuzz_bstring fuzz/corpus/ \
    -max_len=260 \
    -artifact_prefix=fuzz/crashes/ \
    -jobs=$(nproc)
```

Useful libFuzzer flags:

| Flag | Description |
| --- | --- |
| `-max_total_time=N` | Stop after N seconds |
| `-jobs=N` | Run N parallel fuzzing instances |
| `-max_len=4096` | Cap input size in bytes |
| `-artifact_prefix=DIR/` | Directory for crash and timeout inputs |
| `-runs=N` | Stop after N iterations |

## Reproducing a crash

If a crash input is found, reproduce it by passing the file directly to the
relevant target:

```sh
./build-fuzz/fuzz/fuzz_bstring path/to/crash-input
```

## Seed corpus

The `corpus/` directory contains initial seed inputs that guide the fuzzer
toward interesting code paths on the first run.  As the fuzzer discovers new
coverage it writes additional entries to the corpus directory automatically.

## CI (ClusterFuzzLite)

The `.clusterfuzzlite/` directory at the repository root contains the
Dockerfile and `build.sh` used by ClusterFuzzLite.  The GitHub Actions
workflow at `.github/workflows/clusterfuzzlite.yml` runs two jobs:

- **code-change** — 60 seconds on every push and pull request targeting `main`
- **batch** — 1 hour on a weekly schedule

Crash reports from the batch job are surfaced as GitHub Security alerts via
SARIF upload.
