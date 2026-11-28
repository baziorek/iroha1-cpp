#!/usr/bin/env bash
# Packages the Linux build into a tarball for GitHub Releases:
#   <out>/iroha-<version>-linux-x64.tar.gz
#     bin/irohad, iroha-cli (+ iroha_migrate, iroha_wsv_diff)
#     example/...             single-node RocksDB config, genesis, keys
#     iroha-lib/include/...   headers needed to consume iroha-lib
#     iroha-lib/lib/libiroha_all.a   all Iroha static libraries merged
#                                  into a single archive
#     README.txt
#
# Usage (from repo root, after a build):
#   packaging/linux/package-linux.sh [-b build-dir] [-o dist-dir] [-g]
#   -g/--granular  also copy every granular *.a into iroha-lib/lib/static/
set -euo pipefail

BUILD_DIR="build"
OUT_DIR="dist"
GRANULAR=0

while [ $# -gt 0 ]; do
    case "$1" in
        -b|--build-dir) BUILD_DIR="$2"; shift 2 ;;
        -o|--out-dir)   OUT_DIR="$2";   shift 2 ;;
        -g|--granular)  GRANULAR=1;     shift ;;
        -h|--help)      sed -n '2,16p' "$0"; exit 0 ;;
        *) echo "unknown arg: $1" >&2; exit 2 ;;
    esac
done

VERSION=$(git describe --tags --always --dirty 2>/dev/null || echo dev)
VERSION=$(echo "$VERSION" | tr -c 'A-Za-z0-9._-' '-')

STAGE="$OUT_DIR/iroha-$VERSION-linux-x64"
rm -rf "$STAGE"
mkdir -p "$STAGE/bin" "$STAGE/example" "$STAGE/iroha-lib/include" "$STAGE/iroha-lib/lib"

# --- binaries ---
for name in irohad iroha-cli; do
    bin=$(find "$BUILD_DIR" -name "$name" -type f -executable | head -1)
    [ -n "$bin" ] || { echo "binary not found: $name" >&2; exit 1; }
    cp "$bin" "$STAGE/bin/$name"
done
for name in iroha_migrate iroha_wsv_diff; do
    bin=$(find "$BUILD_DIR" -name "$name" -type f -executable | head -1)
    [ -n "$bin" ] && cp "$bin" "$STAGE/bin/$name"
done

# --- single-node example (RocksDB, no PostgreSQL needed) ---
cp example/genesis.block example/node0.pub example/node0.priv \
   example/'admin@test.pub' example/'admin@test.priv' "$STAGE/example/"
cat > "$STAGE/example/config.json" <<'EOF'
{
  "block_store_path" : "blocks",
  "torii_port" : 50051,
  "internal_port" : 10001,
  "database": { "type": "rocksdb", "path": "rocksdb" },
  "max_proposal_size" : 10,
  "vote_delay" : 5000,
  "mst_enable" : false,
  "mst_expiration_time" : 1440,
  "max_rounds_delay": 3000,
  "proposal_creation_timeout": 3000,
  "stale_stream_max_rounds": 2
}
EOF

# --- headers: iroha-lib + the in-tree headers its public headers include ---
# Each tree keeps its own include root (case-insensitive FS safety).
for hdr_root in iroha-lib shared_model libs irohad; do
    [ -d "$hdr_root" ] || continue
    (cd "$hdr_root" && find . -type f \( -name '*.hpp' -o -name '*.h' \)) |
    while read -r rel; do
        mkdir -p "$STAGE/iroha-lib/include/$hdr_root/$(dirname "$rel")"
        cp "$hdr_root/$rel" "$STAGE/iroha-lib/include/$hdr_root/$rel"
    done
done

# --- generated protobuf/gRPC headers (build tree) ---
BUILD_ABS=$(cd "$BUILD_DIR" && pwd)
find "$BUILD_ABS" -type f \( -name '*.pb.h' -o -name '*.grpc.pb.h' \) |
while read -r f; do
    rel="${f#$BUILD_ABS/}"
    mkdir -p "$STAGE/iroha-lib/include/gen/$(dirname "$rel")"
    cp "$f" "$STAGE/iroha-lib/include/gen/$rel"
done

# --- merged static library (built by the iroha_lib_all CMake target) ---
cmake --build "$BUILD_DIR" --target iroha_lib_all --parallel
cp "$BUILD_DIR/lib/libiroha_all.a" "$STAGE/iroha-lib/lib/libiroha_all.a"

if [ "$GRANULAR" -eq 1 ]; then
    mkdir -p "$STAGE/iroha-lib/lib/static"
    find "$BUILD_ABS" -name '*.a' -type f \
        | grep -v '/CMakeFiles/' | grep -v '/test/' |
        while read -r lib; do cp "$lib" "$STAGE/iroha-lib/lib/static/"; done
fi

# --- README ---
cat > "$STAGE/README.txt" <<EOF
Iroha 1 for Linux ($VERSION)
================================

bin/
  irohad            - Iroha node
  iroha-cli         - command-line client (interactive and JSON modes)
  iroha_migrate     - WSV migration tool (PostgreSQL <-> RocksDB)
  iroha_wsv_diff    - WSV comparison tool

example/         - single-node network on RocksDB (no PostgreSQL needed):
                     cd example
                     ../bin/irohad --config config.json \\
                       --genesis_block genesis.block --keypair_name node0
                   then in another console:
                     ../bin/iroha-cli --account_name admin@test

iroha-lib/
  include/       - headers of the C++ client library; add ALL of these
                   include directories, in this order:
                     include/iroha-lib
                     include/shared_model
                     include/libs
                     include/irohad
                     include/gen/schema
  lib/
    libiroha_all.a - ALL Iroha static libraries merged into one archive;
                     link this single file (no link-order guessing)
    static/        - granular per-module archives (only with --granular)

Notes
-----
* libiroha_all.a was built for this Linux toolchain; consumers must use a
  compatible GCC/Clang.
* iroha-lib additionally requires the third-party dependencies
  (protobuf, gRPC, Boost, spdlog, rapidjson, gflags, fmt, OpenSSL) -
  install them with vcpkg or provide your own builds.
* TLS support: pass --tls_root_cert to iroha-cli / use TlsOptions in
  iroha-lib for one-way TLS to torii.
EOF

mkdir -p "$OUT_DIR"
tar -czf "$OUT_DIR/iroha-$VERSION-linux-x64.tar.gz" -C "$OUT_DIR" "iroha-$VERSION-linux-x64"
echo "Created $OUT_DIR/iroha-$VERSION-linux-x64.tar.gz"
