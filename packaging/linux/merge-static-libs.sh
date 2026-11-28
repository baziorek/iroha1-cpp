#!/usr/bin/env bash
# Merge all Iroha static libraries (*.a) from a CMake build tree into a
# single relocatable archive, e.g. libiroha_all.a.
#
# Usage: merge-static-libs.sh <build-dir> <output.a>
#
# Object member names are made unique (prefixed with the archive name)
# before merging, because GNU ar stores only the basename of each member
# and different modules can contain same-named .o files.
set -euo pipefail

BUILD_DIR="${1:?usage: merge-static-libs.sh <build-dir> <output.a>}"
OUTPUT="${2:?usage: merge-static-libs.sh <build-dir> <output.a>}"

command -v ar >/dev/null || { echo "ar not found" >&2; exit 1; }

mapfile -t LIBS < <(find "$(cd "$BUILD_DIR" && pwd)" -name '*.a' -type f \
    | grep -v '/CMakeFiles/' \
    | grep -v '/test/' \
    | sort)

if [ "${#LIBS[@]}" -eq 0 ]; then
    echo "no static libraries found under $BUILD_DIR" >&2
    exit 1
fi

echo "Merging ${#LIBS[@]} archives into $OUTPUT"

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

OUT_OBJS="$TMP/objects"
mkdir -p "$OUT_OBJS"

for lib in "${LIBS[@]}"; do
    name=$(basename "$lib" .a)
    # strip leading 'lib' for readability of member names
    name="${name#lib}"
    src_dir="$TMP/src_$name"
    mkdir -p "$src_dir"
    (cd "$src_dir" && ar x "$lib")
    for obj in "$src_dir"/*; do
        [ -f "$obj" ] || continue
        # unique member name: <archive>__<original>.o
        ln "$obj" "$OUT_OBJS/${name}__$(basename "$obj")" 2>/dev/null \
            || cp "$obj" "$OUT_OBJS/${name}__$(basename "$obj")"
    done
done

ar rcs "$OUTPUT" "$OUT_OBJS"/*
ranlib "$OUTPUT" 2>/dev/null || true

echo "Done: $OUTPUT ($(du -h "$OUTPUT" | cut -f1))"
