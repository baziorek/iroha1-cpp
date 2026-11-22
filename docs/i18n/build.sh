#!/usr/bin/env bash
#
# Builds translated documentation into <output_dir>/<lang>/.
#
# Translations are taken from the archived hyperledger-iroha/iroha-docs-l10n
# repository (per-language Sphinx source trees). Each locale directory is
# overlaid on top of the matching Iroha 1.x English docs snapshot, so pages
# without a translation fall back to English automatically.
#
# To add a new language: make sure docs/locale/<lang>/ exists in the l10n
# repo at L10N_REF and pass the language code as an argument.
#
# Usage: docs/i10n/build.sh <output_dir> [lang ...]
#   e.g. docs/i18n/build.sh docs/_build/html tr
#
set -euo pipefail

OUT_DIR=$1
shift
LANGS=${@:-tr}

L10N_REPO="hyperledger-iroha/iroha-docs-l10n"
L10N_REF="1.2.0"
EN_REPO="hyperledger/iroha"
EN_REF="f2099d499fbf7a0ec8c8df2a4dce100b0bb4349b"  # docs snapshot the l10n repo was made against

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/../.." && pwd)
SRC_DOCS="$REPO_ROOT/docs/source"

# sphinx-build is invoked from a temp dir below, so the output dir must be
# absolute or the translated site would be written under $WORK instead
mkdir -p "$OUT_DIR"
OUT_DIR=$(cd "$OUT_DIR" && pwd)

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

echo ">> Downloading $L10N_REPO@$L10N_REF"
curl -fsSL "https://github.com/$L10N_REPO/archive/$L10N_REF.tar.gz" \
    | tar xz -C "$WORK"
L10N_ROOT=$(find "$WORK" -maxdepth 1 -type d -name 'iroha-docs-l10n-*' | head -1)

echo ">> Downloading $EN_REPO@$EN_REF (docs only needed, full snapshot fetched)"
curl -fsSL "https://github.com/$EN_REPO/archive/$EN_REF.tar.gz" \
    | tar xz -C "$WORK"
EN_ROOT=$(find "$WORK" -maxdepth 1 -type d -name 'iroha-*' ! -name 'iroha-docs-l10n-*' | head -1)

for lang in $LANGS; do
    echo ">> Building '$lang' documentation"
    L10N_SRC="$L10N_ROOT/docs/locale/$lang/docs"
    if [ ! -d "$L10N_SRC/source" ]; then
        echo "!! No translation for '$lang' in $L10N_REPO@$L10N_REF (docs/locale/$lang) — skipped" >&2
        continue
    fi

    DEST="$WORK/$lang/docs"
    mkdir -p "$DEST"

    # Base: English docs snapshot (complete tree; untranslated pages stay English)
    cp -r "$EN_ROOT/docs/source" "$DEST/source"
    mkdir -p "$DEST/image_assets"
    [ -d "$EN_ROOT/docs/image_assets" ] && cp -r "$EN_ROOT/docs/image_assets/." "$DEST/image_assets/"

    # Overlay: translated files only (regular files; symlinks in the l10n repo
    # point into the 'iroha' submodule and are intentionally skipped — the
    # English base already provides those files)
    if [ -d "$L10N_SRC/source" ]; then
        (cd "$L10N_SRC/source" && find . -type f -print0 \
            | tar --null -cf - --files-from=-) \
            | (cd "$DEST/source" && tar xf -)
    fi
    if [ -d "$L10N_SRC/image_assets" ]; then
        (cd "$L10N_SRC/image_assets" && find . -type f -print0 \
            | tar --null -cf - --files-from=-) \
            | (cd "$DEST/image_assets" && tar xf -)
    fi

    # Translators of the l10n repo translated Sphinx directive names
    # (e.g. '.. not::' for '.. note::'), which Sphinx rejects. Map the
    # known mistranslations back to canonical directive names.
    # Mapping verified against the English 1.2.0 sources.
    case "$lang" in
        tr)
            find "$DEST/source" -name '*.rst' -type f -exec sed -i \
                -e 's/^\(\.\. *\)not::/\1note::/I' \
                -e 's/^\(\.\. *\)ipucu::/\1hint::/I' \
                -e 's/^\(\.\. *\)İpucu::/\1hint::/' \
                -e 's/^\(\.\. *\)Ipucu::/\1hint::/' \
                -e 's/^\(\.\. *\)dikkat::/\1attention::/I' \
                -e 's/^\(\.\. *\)uyarı::/\1warning::/I' \
                -e 's/^\(\.\. *\)Uyarı::/\1warning::/' \
                -e 's/^\(\.\. *\)ayrıcabakınız::/\1seealso::/I' \
                {} +
            ;;
    esac

    # Support files are language-neutral: use the current repo's versions so the
    # build behaves exactly like the English one (same theme, extensions, CSS)
    cp "$SRC_DOCS/conf.py" "$DEST/source/conf.py"
    for d in iroha_rst sphinx_extensions _static _extra; do
        rm -rf "$DEST/source/$d"
        [ -d "$SRC_DOCS/$d" ] && cp -r "$SRC_DOCS/$d" "$DEST/source/$d"
    done
    find "$DEST" -name '__pycache__' -type d -exec rm -rf {} + 2>/dev/null || true

    # Mark the locale so Sphinx treats the build as this language
    sed -i "s/^language:.*/language: \"$lang\"/" "$DEST/source/locale.yaml" 2>/dev/null || \
        printf 'language: "%s"\n' "$lang" > "$DEST/source/locale.yaml"

    # example/python is referenced by the iroha_permissions extension for
    # code excerpts (paths are resolved two levels above source/)
    mkdir -p "$WORK/$lang"
    [ -d "$REPO_ROOT/example/python" ] && cp -r "$REPO_ROOT/example/python" "$WORK/$lang/"

    # Locale-level files referenced via relative includes
    # (e.g. community/index.rst includes ../../../CONTRIBUTING.rst,
    # which lives at docs/locale/<lang>/CONTRIBUTING.rst in the l10n repo)
    LOCALE_DIR="$L10N_ROOT/docs/locale/$lang"
    if [ -d "$LOCALE_DIR" ]; then
        (cd "$LOCALE_DIR" && find . -maxdepth 1 -type f -print0 \
            | tar --null -cf - --files-from=-) \
            | (cd "$WORK/$lang" && tar xf -)
    fi

    mkdir -p "$OUT_DIR/$lang"
    (cd "$DEST/source" && sphinx-build -b html . "$OUT_DIR/$lang")
done
