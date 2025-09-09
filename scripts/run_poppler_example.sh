#!/usr/bin/env bash
set -euo pipefail
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <pdf-file>" >&2
  exit 2
fi
PDF="$1"
BUILD_DIR="${PWD}/build"
if [ ! -d "$BUILD_DIR" ]; then
  echo "Build directory not found. Create it and run ./configure && make (see README)." >&2
  exit 3
fi
if [ ! -x "$BUILD_DIR/poppler-example" ]; then
  echo "poppler-example not built. Ensure Poppler was found and built." >&2
  exit 4
fi
"$BUILD_DIR/poppler-example" "$PDF"
