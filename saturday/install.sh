#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST="${REAPER_EFFECTS_DIR:-$HOME/Library/Application Support/REAPER/Effects/reaper-plugins}"

mkdir -p "$DEST"

# Remove old nested install that caused duplicate FX entries
rm -rf "$DEST/saturday"

cp "$SCRIPT_DIR/Saturday.jsfx" "$DEST/"
cp "$SCRIPT_DIR/Saturday.rpl" "$DEST/"

echo "Installed Saturday to:"
echo "  $DEST/Saturday.jsfx"
echo "  $DEST/Saturday.rpl"
echo ""
echo "Restart Reaper or rescan JSFX if needed."
echo "Find the plugin under FX > JS > reaper-plugins > Saturday"
