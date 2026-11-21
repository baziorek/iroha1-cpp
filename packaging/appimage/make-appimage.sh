#!/usr/bin/env bash
## Build an AppImage for a single Iroha binary.
##
## Usage: make-appimage.sh <path-to-binary> <binary-name> <output-file>
##   e.g. make-appimage.sh build/bin/irohad irohad dist/irohad-x86_64.AppImage
##
## Iroha binaries are linked almost statically (vcpkg static linkage):
## the only non-glibc shared deps are libstdc++ and libgcc_s, which are
## bundled into the AppDir so the AppImage runs on older distributions.
set -euo pipefail

readonly binary=$1 appname=$2 output=$3

readonly appdir_root=$(mktemp -d)
readonly appdir=$appdir_root/$appname.AppDir
## repo root is two levels up from this script; do not use git here —
## inside CI containers the checkout may be owned by a different uid and
## 'git rev-parse' fails with "detected dubious ownership"
readonly repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)

mkdir -p "$appdir/bin" "$appdir/usr/lib" "$appdir/usr/share/applications"

cp "$binary" "$appdir/bin/$appname"
chmod 755 "$appdir/bin/$appname"

## Bundle C++ runtime libraries (resolve symlinks to real files)
for lib in libstdc++.so.6 libgcc_s.so.1; do
   src=$(ldconfig -p | awk -v lib="$lib" \
      '$1 == lib && $2 ~ /x86-64/ && !found64 {found64=$NF} \
       $1 == lib && !any {any=$NF} \
       END {print (found64 ? found64 : any)}')
   test -n "$src" || { echo "ERROR: $lib not found via ldconfig" >&2; exit 1; }
   cp -L "$src" "$appdir/usr/lib/$lib"
done

cat >"$appdir/AppRun" <<'EOF'
#!/bin/sh
HERE="${APPDIR:-$(dirname "$(readlink -f "$0")")}"
export LD_LIBRARY_PATH="$HERE/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "$HERE/bin/BINARY_NAME" "$@"
EOF
sed -i "s/BINARY_NAME/$appname/" "$appdir/AppRun"
chmod 755 "$appdir/AppRun"

cat >"$appdir/$appname.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=$appname
Comment=Hyperledger Iroha 1 node
Exec=$appname
Icon=iroha
Categories=Finance;Office;
Terminal=true
EOF

cp "$repo_root/docs/image_assets/iroha_logo.png" "$appdir/iroha.png"

## appimagetool: allow override, otherwise download continuous build
if test -z "${APPIMAGETOOL:-}" ;then
   APPIMAGETOOL=$appdir_root/appimagetool
   curl -fL -o "$APPIMAGETOOL" \
      https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage
   chmod +x "$APPIMAGETOOL"
fi

mkdir -p "$(dirname "$output")"
ARCH=x86_64 APPIMAGE_EXTRACT_AND_RUN=1 "$APPIMAGETOOL" "$appdir" "$output"

echo "Created $output"
rm -rf "$appdir_root"
