#!/usr/bin/env bash
set -euo pipefail

# Packages built plugin binaries into dist/Saturday-<platform>.zip
# Usage: ./scripts/package-release.sh <macos|windows|linux> [build_dir]

PLATFORM="${1:?Usage: $0 <macos|windows|linux> [build_dir]}"
BUILD_DIR="${2:-build}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ARTEFACTS="$ROOT/$BUILD_DIR/Saturday_artefacts/Release"
DIST="$ROOT/dist"
STAGE="$DIST/stage-$PLATFORM"

rm -rf "$STAGE"
mkdir -p "$STAGE" "$DIST"

case "$PLATFORM" in
  macos)
    cp -R "$ARTEFACTS/VST3/Saturday.vst3" "$STAGE/"
    cp -R "$ARTEFACTS/AU/Saturday.component" "$STAGE/"
    cp "$ROOT/scripts/install-macos.sh" "$STAGE/"
    cat > "$STAGE/INSTALL.txt" <<'EOF'
Saturday — macOS install

Copy the plugins into your user plug-in folders:

  Saturday.vst3     → ~/Library/Audio/Plug-Ins/VST3/
  Saturday.component → ~/Library/Audio/Plug-Ins/Components/

Then rescan plug-ins in your DAW (or restart Reaper / Logic).

Quick install from this folder:

  chmod +x install-macos.sh && ./install-macos.sh
EOF
    ;;
  windows)
    cp -R "$ARTEFACTS/VST3/Saturday.vst3" "$STAGE/"
    cat > "$STAGE/INSTALL.txt" <<'EOF'
Saturday — Windows install

Copy Saturday.vst3 to your VST3 folder, for example:

  %LOCALAPPDATA%\Programs\Common\VST3\

Or your DAW's custom VST3 path. Then rescan plug-ins in the DAW.

PowerShell example:

  Copy-Item -Recurse Saturday.vst3 "$env:LOCALAPPDATA\Programs\Common\VST3\"
EOF
    ;;
  linux)
    cp -R "$ARTEFACTS/VST3/Saturday.vst3" "$STAGE/"
    cp "$ROOT/scripts/install-linux.sh" "$STAGE/"
    cat > "$STAGE/INSTALL.txt" <<'EOF'
Saturday — Linux install

Copy Saturday.vst3 to your user VST3 folder:

  ~/.vst3/Saturday.vst3

Or system-wide:

  /usr/lib/vst3/Saturday.vst3

Then rescan in Reaper (Preferences → Plug-ins → VST) or restart your DAW.

Example:

  chmod +x install-linux.sh && ./install-linux.sh
EOF
    ;;
  *)
    echo "Unknown platform: $PLATFORM" >&2
    exit 1
    ;;
esac

ZIP="$DIST/Saturday-${PLATFORM}.zip"
rm -f "$ZIP"
(
  cd "$STAGE"
  if command -v zip >/dev/null 2>&1; then
    zip -r "$ZIP" .
  else
    powershell -Command "Compress-Archive -Path * -DestinationPath '$ZIP' -Force"
  fi
)

echo "Created $ZIP"
ls -lh "$ZIP"
