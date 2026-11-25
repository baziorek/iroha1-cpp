#!/usr/bin/env bash
#
# Builds translations that live in THIS repository under docs/i18n/<lang>/.
#
# Unlike build.sh (which overlays per-language source trees downloaded
# from the archived hyperledger-iroha/iroha-docs-l10n repo onto an old
# Iroha 1.2.0 English snapshot), this script overlays docs/i18n/<lang>/
# onto the CURRENT docs/source tree. Pages without a translation file
# fall back to the current English version automatically.
#
# Layout of a translation:
#   docs/i18n/<lang>/source/**.rst   - translated Sphinx sources
#   docs/i18n/<lang>/CONTRIBUTING.rst (optional) - locale-level files that
#     community/index.rst pulls in via a relative include
#
# Usage: docs/i18n/build-local.sh <output_dir> <lang> [lang ...]
#   e.g. docs/i18n/build-local.sh docs/_build/html pl
#
set -euo pipefail

OUT_DIR=$1
shift
LANGS=${@:?usage: build-local.sh <output_dir> <lang> [lang ...]}

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/../.." && pwd)
SRC_DOCS="$REPO_ROOT/docs/source"
I18N_ROOT="$REPO_ROOT/docs/i18n"

mkdir -p "$OUT_DIR"
OUT_DIR=$(cd "$OUT_DIR" && pwd)

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

for lang in $LANGS; do
    L10N_SRC="$I18N_ROOT/$lang/source"
    if [ ! -d "$L10N_SRC" ]; then
        echo "!! No local translation for '$lang' ($L10N_SRC missing) — skipped" >&2
        continue
    fi

    echo ">> Building local '$lang' documentation"
    DEST="$WORK/$lang/docs"
    mkdir -p "$DEST"

    # Base: the CURRENT English docs (complete tree; untranslated pages
    # stay English)
    cp -r "$SRC_DOCS" "$DEST/source"
    mkdir -p "$DEST/image_assets"
    [ -d "$REPO_ROOT/docs/image_assets" ] && cp -r "$REPO_ROOT/docs/image_assets/." "$DEST/image_assets/"

    # Overlay: translated files only
    (cd "$L10N_SRC" && find . -type f -print0 \
        | tar --null -cf - --files-from=-) \
        | (cd "$DEST/source" && tar xf -)

    # Repo files referenced by literalinclude/include directives
    # (paths like ../../../vcpkg/... resolve two levels above source/,
    # i.e. to the build root $WORK/$lang/). Symlink them so translated
    # builds see the same files as the English one.
    for p in vcpkg example shared_model iroha-lib libs irohad; do
        [ -e "$REPO_ROOT/$p" ] && ln -sfn "$REPO_ROOT/$p" "$WORK/$lang/$p"
    done

    # Locale-level files referenced via relative includes
    # (community/index.rst includes ../../../CONTRIBUTING.rst, resolved
    # against the build root two levels above source/)
    LOCALE_DIR="$I18N_ROOT/$lang"
    mkdir -p "$WORK/$lang"
    if [ -f "$LOCALE_DIR/CONTRIBUTING.rst" ]; then
        ln -sfn "$LOCALE_DIR/CONTRIBUTING.rst" "$WORK/$lang/CONTRIBUTING.rst"
    else
        ln -sfn "$REPO_ROOT/CONTRIBUTING.rst" "$WORK/$lang/CONTRIBUTING.rst"
    fi

    # Mark the locale so Sphinx treats the build as this language
    sed -i "s/^language:.*/language: \"$lang\"/" "$DEST/source/locale.yaml" 2>/dev/null || \
        printf 'language: "%s"\n' "$lang" > "$DEST/source/locale.yaml"

    find "$DEST" -name '__pycache__' -type d -exec rm -rf {} + 2>/dev/null || true

    mkdir -p "$OUT_DIR/$lang"
    (cd "$DEST/source" && sphinx-build -b html . "$OUT_DIR/$lang")
    echo ">> '$lang' documentation built into $OUT_DIR/$lang"
done
