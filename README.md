# MXTune

[![Build and Release](https://github.com/alexander-shch/MXTune-reborn/actions/workflows/release.yml/badge.svg)](https://github.com/alexander-shch/MXTune-reborn/actions/workflows/release.yml)

An open-source auto-tune / pitch correction audio plugin (VST3/AU) with real-time pitch detection and correction to a user-defined musical scale or MIDI-driven note mapping.

## Table of Contents

- [Download](#download)
- [Installation](#installation)
- [Usage](#usage)
- [Building from Source](./docs/BUILDING.md)
- [Roadmap](https://github.com/users/alexander-shch/projects/7)
- [Developer Notes](./docs/DEVELOPER_NOTES.md)
- [Troubleshooting](./docs/TROUBLESHOOTING.md)
- [Changelog](./CHANGELOG.md)
- [Credits](#credits)

---

## Download

> Stable releases for macOS, Windows, and Linux are published on each version tag push.

| Platform | Download |
|---|---|
| macOS | [Latest release](https://github.com/alexander-shch/MXTune-reborn/releases/latest) |
| Windows | [Latest release](https://github.com/alexander-shch/MXTune-reborn/releases/latest) |
| Linux | [Latest release](https://github.com/alexander-shch/MXTune-reborn/releases/latest) |

All releases: [github.com/alexander-shch/MXTune-reborn/releases](https://github.com/alexander-shch/MXTune-reborn/releases)

---

## Installation

### macOS
Extract the `.zip`:
- **VST3**: move `mx_tune-*.vst3` to `/Library/Audio/Plug-Ins/VST3/`
- **AU**: move `mx_tune-*.component` to `/Library/Audio/Plug-Ins/Components/`

Rescan plugins in your DAW.

### Windows
Extract the `.zip`, move the `.vst3` folder to `C:\Program Files\Common Files\VST3\`.

### Linux
Extract the `.zip`, move the `.vst3` folder to `~/.vst3/` or `/usr/lib/vst3/`.

---

## Usage

### Hotkeys

| Input | Action |
|:------|:-------|
| `alt + mouse wheel` | Zoom X |
| `ctrl + mouse wheel` | Zoom Y |
| `mouse wheel` or `W / S` | Move Y |
| `shift + mouse wheel` or `A / D` | Move X |
| `left button drag` | Add note |
| `right button` | Delete note |

---

## Roadmap

The next major milestone is **ARA (Audio Random Access)** support, enabling full-clip pitch analysis without requiring the host to play audio through the plugin. This will be the foundation for vibrato control, syllable time-stretching, and multi-instance pitch grid overlays.

For the full execution plan, phase breakdown, and testing strategy, see the **[project board](https://github.com/users/alexander-shch/projects/7)**.

---

## Credits

- **Original author:** [Liu An Lin (liuanlin-mx)](https://github.com/liuanlin-mx/MXTune) — core pitch detection and correction algorithm, based on Tom Baran's [Autotalent](http://web.mit.edu/tbaran/www/autotalent.html) and [TalentedHack](http://code.google.com/p/talentledhack/)
- **macOS build structure:** [Hammond95](https://github.com/Hammond95/MXTune) — macOS CMake setup, Projucer integration, and macOS 15+ compatibility patch
- JUCE is licensed under the GPL v3 or a commercial license. See [juce.com](https://juce.com) for more details.
