# Saturday — VST3 / AU / Standalone

Native macOS plugin with the full boutique UI. DSP matches **`../Saturday.jsfx`**.

**Developer:** Whatever555 · **Version:** 1.0.0

The Reaper JSFX build keeps a simpler UI and includes 29 factory presets in `Saturday.rpl`. Use this native build when you want the full UI or need Saturday outside Reaper.

## Formats

| Format | Extension | Typical use |
|--------|-----------|-------------|
| **AU** | `.component` | Logic, GarageBand, Reaper (macOS) |
| **VST3** | `.vst3` | Reaper, Ableton, most DAWs |
| **Standalone** | `.app` | Quick testing without a DAW |

## Requirements

- macOS with Xcode command-line tools (`xcode-select --install`)
- CMake 3.22+ (`brew install cmake`)

JUCE 8.0.6 is fetched automatically on first configure.

## Build

From this directory (`saturday/plugin/`):

```bash
cmake -B build -G Xcode
xcodebuild -project build/Saturday.xcodeproj -scheme Saturday_AU -configuration Release -jobs 8
xcodebuild -project build/Saturday.xcodeproj -scheme Saturday_VST3 -configuration Release -jobs 8
```

Optional standalone app:

```bash
xcodebuild -project build/Saturday.xcodeproj -scheme Saturday_Standalone -configuration Release -jobs 8
```

First build downloads JUCE (~1–2 min). Use **Xcode** as the generator — Unix Makefiles have failed on some CMake versions on macOS.

## Install locations

`COPY_PLUGIN_AFTER_BUILD` is enabled in `CMakeLists.txt`, so Release builds copy plugins automatically:

| Format | Path |
|--------|------|
| **AU** | `~/Library/Audio/Plug-Ins/Components/Saturday.component` |
| **VST3** | `~/Library/Audio/Plug-Ins/VST3/Saturday.vst3` |
| **Standalone** | `build/Saturday_artefacts/Release/Standalone/Saturday.app` |

**Manual install:** Copy the built `.component` or `.vst3` from `build/Saturday_artefacts/Release/` into the paths above.

After installing, **rescan plugins** in your DAW (or restart Reaper / Logic).

## Load in a DAW

| DAW | Menu path |
|-----|-----------|
| **Reaper** | FX → VST3: Whatever555 → Saturday, or AU: Whatever555 → Saturday |
| **Logic** | Audio FX → AU → Whatever555 → Saturday |
| **Ableton** | Audio Effects → VST3 → Whatever555 → Saturday |

If Saturday does not appear, confirm the file exists under `~/Library/Audio/Plug-Ins/` and trigger a plug-in scan.

## Controls

Same parameters as the JSFX version. Mode buttons use the same names as the DSP:

| UI | Parameter | Range / values |
|----|-----------|----------------|
| **Cassette** | Mode | Cassette (0) |
| **Tube** | Mode | Tube (1) |
| **Iconic** | Mode | Iconic (2) |
| **DRIVE** | Drive | 0–100% |
| **MIX** | Mix | 0–100% |
| **TONE** | Tone | −100 to +100 |
| **GATE** | Gate | −80 dB (off) to 0 dB — default −28 dB |
| **QUALITY** | Quality | Standard / Hi-Fi / Ultra (oversampling) |

**IN** and **OUT** meters show input and output peak levels.

## Presets

Factory presets from `Saturday.rpl` are **JSFX-only** for now. On VST3/AU, save and recall settings through your DAW’s preset system.

## Project layout

```
plugin/
  CMakeLists.txt          Build config (JUCE FetchContent)
  Source/
    PluginProcessor.*     APVTS parameters, audio I/O
    PluginEditor.*        Custom UI
    SaturdayDSP.*         DSP (ported from Saturday.jsfx)
  Assets/
    design-reference.png  UI mockup reference
```

## Troubleshooting

- **Build fails with CMake + Makefiles** — Regenerate with `-G Xcode`.
- **Stale UI after rebuild** — Remove the plugin from the track and insert a fresh instance.
- **Code signing / gatekeeper (Standalone)** — Right-click → Open the first time, or sign with your Apple ID in Xcode if distributing.

## Reaper JSFX

Install separately from the parent folder:

```bash
cd ..
chmod +x install.sh
./install.sh
```

See **[../README.md](../README.md)** for JSFX install, factory presets, and usage.
