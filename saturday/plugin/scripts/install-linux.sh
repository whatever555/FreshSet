#!/usr/bin/env bash
set -euo pipefail

# Install Saturday VST3 from the current directory (after unzipping a release).
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST="${VST3_DIR:-${HOME}/.vst3}"

mkdir -p "$DEST"

if [[ ! -d "$ROOT/Saturday.vst3" ]]; then
  echo "Saturday.vst3 not found next to this script." >&2
  exit 1
fi

rm -rf "$DEST/Saturday.vst3"
cp -R "$ROOT/Saturday.vst3" "$DEST/"
echo "Installed → $DEST/Saturday.vst3"
echo "Rescan VST3 plug-ins in your DAW."
