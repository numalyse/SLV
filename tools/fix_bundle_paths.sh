#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <path to .app>" >&2
  exit 2
fi

BUNDLE="$1"
echo "Fixing bundle paths in: ${BUNDLE}"

RES_PY="$BUNDLE/Contents/Resources/python"
DLIB_DIR="$RES_PY/.dylibs"
mkdir -p "$DLIB_DIR"

echo "Scanning for absolute non-system dependencies..."

find "$BUNDLE" -type f \( -name '*.dylib' -o -name '*.so' -o -name '*.so.*' -o -perm -111 -o -name '*python*' \) | while read -r file; do
  otool -L "$file" 2>/dev/null | tail -n +2 | awk '{print $1}' | while read -r dep; do
    # skip @rpath/@loader_path and system libs
    case "$dep" in
      @*|/usr/lib/*|/System/*|/usr/lib/system/*)
        continue
        ;;
    esac

    # only handle absolute existing files; if missing, try to locate under common prefixes
    if [ -f "$dep" ]; then
      bn=$(basename "$dep")
      target="$DLIB_DIR/$bn"
      if [ ! -f "$target" ]; then
        echo "Copying $dep -> $target"
        cp -L "$dep" "$target"
        chmod 644 "$target" || true
        echo "Setting id on $target"
        install_name_tool -id "@executable_path/../Resources/python/.dylibs/$bn" "$target" || true
      fi

      echo "Patching $file: $dep -> @executable_path/../Resources/python/.dylibs/$bn"
      install_name_tool -change "$dep" "@executable_path/../Resources/python/.dylibs/$bn" "$file" || true
    else
      # try to locate by basename under several common prefixes
      bn=$(basename "$dep")
      echo "Dependency $dep not found; searching for $bn..."
      FOUND=""
      search_prefixes=(/opt/homebrew /usr/local /usr /Library/Frameworks /DLC /opt/local)
      for p in "${search_prefixes[@]}"; do
        if [ -d "$p" ]; then
          found=$(find "$p" -type f -name "$bn" -print -quit 2>/dev/null || true)
          if [ -n "$found" ]; then
            FOUND="$found"
            break
          fi
        fi
      done

      if [ -n "$FOUND" -a -f "$FOUND" ]; then
        echo "Found $bn at $FOUND — copying and patching"
        target="$DLIB_DIR/$bn"
        if [ ! -f "$target" ]; then
          cp -L "$FOUND" "$target"
          chmod 644 "$target" || true
          install_name_tool -id "@executable_path/../Resources/python/.dylibs/$bn" "$target" || true
        fi
        install_name_tool -change "$dep" "@executable_path/../Resources/python/.dylibs/$bn" "$file" || true
      else
        echo "Referenced dependency not found on disk: $dep (from $file) — not located under common prefixes"
      fi
    fi
  done
done

echo "Resigning bundle (ad-hoc)"
codesign --force --deep --sign - "$BUNDLE" || true

echo "Done fixing bundle paths."
