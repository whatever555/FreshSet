#!/usr/bin/env bash
set -euo pipefail

# Install Saturday VST3 + AU from the current directory (after unzipping a release).
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

VST3_DEST="${HOME}/Library/Audio/Plug-Ins/VST3"
AU_DEST="${HOME}/Library/Audio/Plug-Ins/Components"

mkdir -p "$VST3_DEST" "$AU_DEST"

if [[ -d "$ROOT/Saturday.vst3" ]]; then
  rm -rf "$VST3_DEST/Saturday.vst3"
  cp -R "$ROOT/Saturday.vst3" "$VST3_DEST/"
  echo "Installed VST3 → $VST3_DEST/Saturday.vst3"
else
  echo "Saturday.vst3 not found next to this script." >&2
  exit 1
fi

if [[ -d "$ROOT/Saturday.component" ]]; then
  rm -rf "$AU_DEST/Saturday.component"
  cp -R "$ROOT/Saturday.component" "$AU_DEST/"
  echo "Installed AU → $AU_DEST/Saturday.component"
fi

echo "Rescan plug-ins in your DAW (or restart Reaper / Logic)."
