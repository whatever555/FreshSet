#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST="${REAPER_EFFECTS_DIR:-$HOME/Library/Application Support/REAPER/Effects/reaper-plugins}"

mkdir -p "$DEST"

# Remove old nested install that caused duplicate FX entries
rm -rf "$DEST/saturday"

cp "$SCRIPT_DIR/Saturday.jsfx" "$DEST/"
cp "$SCRIPT_DIR/Saturday.rpl" "$DEST/"
cp "$SCRIPT_DIR/Saturday.rpl" "$DEST/Saturday.jsfx.rpl"
# Fallback library name (some Reaper builds match desc only)
if [[ -f "$SCRIPT_DIR/Saturday-alt.rpl" ]]; then
  cp "$SCRIPT_DIR/Saturday-alt.rpl" "$DEST/Saturday-alt.rpl"
fi

echo "Installed Saturday to:"
echo "  $DEST/Saturday.jsfx"
echo "  $DEST/Saturday.rpl"
echo "  $DEST/Saturday.jsfx.rpl"
echo ""
echo "Restart Reaper or rescan JSFX if needed."
echo "Presets appear in the FX window preset dropdown (+ to save)."
echo "Find the plugin under FX > JS > reaper-plugins > Saturday"
