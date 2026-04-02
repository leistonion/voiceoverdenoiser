#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$ROOT_DIR/dist"
OUT_NAME="voiceoverdenoiser_project.zip"
OUT_PATH="$OUT_DIR/$OUT_NAME"

mkdir -p "$OUT_DIR"

cd "$ROOT_DIR"
zip -r "$OUT_PATH" . \
  -x ".git/*" \
     "build/*" \
     "dist/*" \
     "*.DS_Store"

echo "Created: $OUT_PATH"
