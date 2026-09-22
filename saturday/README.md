# Saturday

A beautiful, authentic-sounding multi-mode saturation plugin for Reaper. Adds warmth and character with a simple interface and gorgeous custom UI.

**Modes:** Tape · Tube · Transformer

## Install

### macOS (automatic)

```bash
chmod +x install.sh
./install.sh
```

This copies the plugin to:

```
~/Library/Application Support/REAPER/Effects/reaper-plugins/
  Saturday.jsfx
  Saturday.rpl
```

### Manual install

Copy `Saturday.jsfx` and `Saturday.rpl` into your Reaper Effects folder (same directory):

- macOS: `~/Library/Application Support/REAPER/Effects/reaper-plugins/`
- Windows: `%APPDATA%\REAPER\Effects\reaper-plugins\`
- Linux: `~/.config/REAPER/Effects/reaper-plugins/`

**Important:** Do not put `.preset` files inside the Effects folder — Reaper will list each one as a separate plugin. Factory presets ship in the single `Saturday.rpl` bank file.

If you previously installed to `reaper-plugins/saturday/`, delete that folder and reinstall.

Rescan JSFX in Reaper if the plugin does not appear immediately.

## Usage

1. Add **FX → JS → reaper-plugins → saturday → Saturday** to a track or bus.
2. Open the plugin window for the custom UI.
3. Choose a mode, set Drive, and adjust Tone/Mix/Output to taste.
4. Load factory presets from the Reaper preset dropdown.

## Controls

| Control | Description |
|---------|-------------|
| **Mode** | Tape (soft, compressed), Tube (punchy, even harmonics), Transformer (thick, weighty) |
| **Drive** | Saturation amount (0–100%) |
| **Tone** | Tilt EQ — dark/warm (−) to bright (+) |
| **Mix** | Wet/dry blend (0% = dry bypass, 100% = full saturation) |
| **Output** | Output level (−12 to +12 dB) |
| **Auto Gain** | Compensates loudness as Drive increases |
| **Oversampling** | Off / 2× / 4× — reduces aliasing on heavy saturation |

### UI tips

- **Drag** knobs vertically to adjust values.
- **Ctrl+click** a knob to reset to default (Drive/Tone/Output → 0, Mix → 100%).
- **Click** mode buttons to switch character (crossfaded to prevent clicks).

## Factory presets

| Preset | Mode | Best for |
|--------|------|----------|
| Warm Tape | Tape | Vocals, keys, general warmth |
| Vocal Glow | Tube | Lead vocals, presence |
| Drum Punch | Tube | Drums, transients |
| Mix Glue | Tape | Bus glue, subtle cohesion |
| Transformer Weight | Transformer | Bass, low-end thickness |
| Subtle Warmth | Tape | Transparent warmth |
| Heavy Tape | Tape | Aggressive tape crush |
| Bright Tube | Tube | Bright, forward saturation |
| Bass Thickener | Transformer | Bass guitar, sub weight |
| Master Saturday | Tape | Master bus, gentle finish |
| Acoustic Air | Tube | Acoustic guitar, strings |
| Lo-Fi Crush | Tape | Lo-fi, degraded character |
| Parallel Sat | Tube | New York-style parallel saturation |
| Bus Character | Transformer | Mix bus weight |
| Synth Heat | Tube | Synthesizers, pads |

## Technical notes

- All parameters are smoothed to prevent zipper noise.
- Nonlinear processing supports 2×/4× internal oversampling.
- Mode switches are crossfaded over 128 samples.
- DC blocking and safety limiting protect against unstable output.
- Default settings (Drive 0, Mix 100%) pass audio cleanly with minimal coloration.

## QA checklist

Before release, verify in Reaper:

- [ ] Default state passes audio without runaway gain
- [ ] Drive sweeps 0→100 on sine, drums, vocals — no clicks
- [ ] Mode switching during playback — no pops
- [ ] Sample rates: 44.1, 48, 96, 192 kHz
- [ ] Extreme inputs (silence, hot signal) — no NaN or harsh output
- [ ] Multiple instances on one project — stable CPU
- [ ] Preset load/save round-trip — values match
- [ ] Mono track input — correct stereo behavior
