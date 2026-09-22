# Saturday

A beautiful, authentic-sounding multi-mode saturation plugin for Reaper. Adds warmth and character with a simple interface and gorgeous custom UI.

**Modes:** Cassette · Tube · Iconic

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
3. Choose a mode, set Drive, and adjust Tone/Mix to taste.
4. Load factory presets from the **preset dropdown** at the top of the FX window (29 included). If empty after install, remove and re-add the FX, or rescan JSFX.

## Controls

| Control | Description |
|---------|-------------|
| **Mode** | Cassette (worn vintage — wow, glue, muffled highs), Tube (mid-forward warmth), Iconic (aura glow — presence halo on words) |
| **Drive** | Saturation amount (0–100%) |
| **Tone** | Tilt EQ — dark/warm (−) to bright (+) |
| **Mix** | Wet/dry blend (0% = dry bypass, 100% = full saturation) |
| **Gate** | Threshold slider (−80 dB = off, default −28 dB) — saturation only above level |
| **Quality** | Standard / Hi-Fi / Ultra — higher settings reduce aliasing (more CPU) |

### UI tips

- **Drag** knobs vertically to adjust values.
- **Ctrl+click** a knob to reset to default (Drive → 35%, Tone → 0, Mix → 100%).
- **Double-click** Gate to reset to default (−28 dB).
- **Click** mode buttons to switch character (crossfaded to prevent clicks).

### Mode A/B test

To hear the difference clearly:

1. Gate **Off** (drag fully left, or load **Cassette Night** / **Vocal Glow** / **Neon Presence**)
2. Drive **40%**, Mix **100%**, Tone **0**
3. Switch Cassette → Tube → Iconic on the same vocal phrase

Cassette should sound worn and vintage — wobbly low end, tape glue, scooped mids, dull highs; Tube mid-forward and warm; Iconic adds a polished air/presence halo that opens on louder syllables.

## Factory presets

Presets set exactly what you see in the custom UI, plus **Quality** and **Gate**:

| Preset field | Visible control |
|--------------|-----------------|
| Mode | Cassette / Tube / Iconic button |
| Drive, Tone, Mix | Knobs |
| Quality | Hi-Fi / Standard / Ultra button |
| Gate | Gate slider |

Hidden legacy sliders (Output, Auto Gain) are ignored — they no longer affect sound.

Gate defaults to −28 dB. Showcase presets (**Cassette Night**, **Vocal Glow**, **Neon Presence**, **Lo-Fi Crush**) use Gate Off (−80) for obvious mode character.

**Note:** Some presets use Mix below 100% (e.g. **Acoustic Air** 60%, **Parallel Sat** 45%) — that is intentional parallel blending, not a bug.

### Vocals

| Preset | Mode | Best for |
|--------|------|----------|
| Warm Tape | Tape | General vocal warmth |
| Vocal Glow | Iconic | Gentle aura layer — 18% drive, 40% mix |
| Neon Presence | Iconic | Forward pop presence; 72% mix |
| Quiet Line | Tape | Voiceover; warmth on words only |
| Ribbon Vocal | Tube | Smooth, dark vocal body |

### Drums & room

| Preset | Mode | Best for |
|--------|------|----------|
| Drum Punch | Tube | Kick and snare punch |
| Snare Crack | Tube | Snare transient bite |
| Room Glue | Tube | Overhead / room glue (68% mix) |

### Bass, guitar & DI

| Preset | Mode | Best for |
|--------|------|----------|
| Bass Thickener | Tube | Bass guitar weight |
| Tube Weight | Tube | Low-end density |
| Direct Heat | Tube | DI warmth and body |
| 12AX7 Push | Tube | Pushed guitar preamp mids |

### Keys, synth & strings

| Preset | Mode | Best for |
|--------|------|----------|
| Acoustic Air | Tube | Acoustic guitar presence |
| Sunlit Strings | Iconic | String shimmer (58% mix) |
| Synth Heat | Tube | Pads and synth harmonics |
| Icon Driver | Iconic | Aggressive synth lead |
| Cassette Night | Tape | Worn cassette on keys |

### Bus, mix & master

| Preset | Mode | Best for |
|--------|------|----------|
| Mix Glue | Tape | Group bus cohesion |
| Bus Character | Tube | Mix bus weight |
| Console Smoke | Tube | Parallel bus haze (38% mix) |
| Parallel Sat | Tube | NY parallel saturation |
| Bloom Stack | Iconic | Stack / bus harmonic bloom |
| Master Saturday | Tape | Master bus finish |
| Tape Print | Tape | Final print (4× OS) |

### Character & lo-fi

| Preset | Mode | Best for |
|--------|------|----------|
| Subtle Warmth | Tape | Transparent color |
| 2 Inch Machine | Tape | Classic fat tape |
| Heavy Tape | Tape | Pushed tape crush |
| Lo-Fi Crush | Tape | Aggressive degradation |
| Bright Tube | Tube | Bright, forward saturation |

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
