# Saturday — VST3 / AU / Standalone

Native plugin with the full boutique UI. DSP matches **`../Saturday.jsfx`**.

**Developer:** Whatever555 · **Version:** 1.0.0

## Download (recommended)

No Xcode or compiler needed — grab a pre-built zip for your OS:

**[GitHub Releases →](https://github.com/whatever555/Saturday/releases)**

| Platform | Zip | Plug-in formats |
|----------|-----|-----------------|
| macOS | `Saturday-macos.zip` | VST3 + AU (universal: Apple Silicon & Intel) |
| Windows | `Saturday-windows.zip` | VST3 |
| Linux | `Saturday-linux.zip` | VST3 |

Each zip includes `INSTALL.txt` and a one-line install script where applicable.

### Install paths

| OS | VST3 | AU (macOS only) |
|----|------|-----------------|
| **macOS** | `~/Library/Audio/Plug-Ins/VST3/` | `~/Library/Audio/Plug-Ins/Components/` |
| **Windows** | `%LOCALAPPDATA%\Programs\Common\VST3\` | — |
| **Linux** | `~/.vst3/` (or `/usr/lib/vst3/` system-wide) | — |

After copying, **rescan plug-ins** in your DAW.

## Load in a DAW

| DAW | Menu path |
|-----|-----------|
| **Reaper** | FX → VST3: Whatever555 → Saturday (macOS: AU also available) |
| **Logic** | Audio FX → AU → Whatever555 → Saturday |
| **Ableton** | Audio Effects → VST3 → Whatever555 → Saturday |

## Controls

Same parameters as the JSFX version:

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

## Build from source

For developers only. CI builds all platforms automatically on push to `main`; tagged releases (`v*` or `plugin-v*`) publish the zips.

### macOS

```bash
brew install cmake ninja
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel --target Saturday_VST3 Saturday_AU
./scripts/package-release.sh macos
```

Or with Xcode: `cmake -B build -G Xcode` then `xcodebuild …`

### Windows

```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel --target Saturday_VST3
bash scripts/package-release.sh windows
```

Requires Visual Studio 2022 build tools (preinstalled on a typical dev machine).

### Linux (Ubuntu 22.04+)

```bash
sudo apt-get install -y build-essential ninja-build cmake \
  libasound2-dev libjack-jackd2-dev ladspa-sdk libcurl4-openssl-dev \
  libfreetype6-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev \
  libxinerama-dev libxrandr-dev libxrender-dev libgl1-mesa-dev libglu1-mesa-dev \
  libwebkit2gtk-4.0-dev
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel --target Saturday_VST3
./scripts/package-release.sh linux
```

Built artefacts land in `build/Saturday_artefacts/Release/`. Set `-DSATURDAY_COPY_AFTER_BUILD=OFF` on CI; leave default `ON` for local installs into system folders.

## Project layout

```
plugin/
  CMakeLists.txt
  scripts/
    package-release.sh    Zip artefacts for distribution
    install-macos.sh      Copy VST3 + AU to ~/Library/Audio/Plug-Ins/
    install-linux.sh      Copy VST3 to ~/.vst3/
  Source/
    PluginProcessor.*     APVTS parameters, audio I/O
    PluginEditor.*        Custom UI
    SaturdayDSP.*         DSP (ported from Saturday.jsfx)
  Assets/
    design-reference.png  UI mockup reference
```

## Troubleshooting

- **Plug-in not listed** — Confirm the file is in the correct folder and rescan.
- **Stale UI after update** — Remove the FX instance and insert a fresh one.
- **macOS Gatekeeper** — If blocked, right-click → Open, or build/sign locally.
- **Linux** — Built on Ubuntu 22.04; other distros may need the dev libraries above to build, but the release binary should run on most recent glibc-based systems.

## Reaper JSFX

Install separately from the parent folder:

```bash
cd ..
chmod +x install.sh
./install.sh
```

See **[../README.md](../README.md)** for JSFX install, factory presets, and usage.
