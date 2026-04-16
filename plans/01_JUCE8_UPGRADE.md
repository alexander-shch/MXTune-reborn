# JUCE 8 Upgrade

**Status: Done — merged to `master` 2026-04-15, released as v2.0.0.**

> Prerequisite for ARA work — met. See `02_ARA_IMPLEMENTATION.md`.

---

## What Was Actually Delivered

The scope expanded well beyond the original version-bump plan. All items below
were delivered in a single squashed commit on `feature/juce8-upgrade`.

| Item | Original Plan | Actual |
|:-----|:-------------|:-------|
| JUCE version | 7.0.5 → 8.0.12 | ✅ Done |
| Build system | Simple version bump in CI | ✅ **Full migration to JUCE native CMake** — Projucer dropped entirely |
| VST3 SDK | Remove `VST3_SDK/` submodule | ✅ Done (JUCE 8 bundles VST3 SDK internally) |
| CMake minimum | 3.15 → 3.22 | ✅ Done |
| State serialization | No change planned | ✅ **kvbuf → JUCE ValueTree** (binary; JSON legacy still readable) |
| Obsolete files | No cleanup planned | ✅ **15 files removed**: LV2 wrappers, standalone app files, Projucer `.jucer`, old build scripts, CodeLite IDE files |
| CI | Minor URL update | ✅ **Full rewrite**: Projucer `--resave` steps removed; Linux/macOS use pkg-config + Ninja + ccache; Windows uses MSVC + vcpkg; JUCE downloaded as a platform-agnostic source archive |
| Version | No bump planned | ✅ Bumped to **2.0.0** — reflects the breaking build-system change |

---

## Original Goal

Replace JUCE 7.0.5 with JUCE 8.0.12 while keeping the existing plugin
behaviour completely unchanged.

**Done when:** Clean three-platform CI build passes, and the plugin loads and
processes audio correctly in Logic Pro and Reaper with no regression.

---

## Changes Made

### Build System — JUCE Native CMake

Replaced the 414-line Projucer-generated `CMakeLists.txt` with a 148-line
clean `juce_add_plugin()` build. Everything manually duplicated by Projucer
(JuceLibraryCode generation, plugin client wrappers, AU enable hacks) is now
handled natively by JUCE CMake.

Key structure:
```cmake
add_subdirectory(third_party/JUCE)
juce_add_plugin(MXTune
    COMPANY_NAME "liuanlin-mx"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE MXTn
    FORMATS VST3 AU   # AU only on macOS
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    ...
)
juce_generate_juce_header(MXTune)
```

Cross-platform dependency split (unavoidable — MinGW is hard-blocked by JUCE):
- **Linux/macOS**: `pkg_check_modules()` via PkgConfig
- **Windows**: `find_package` / `find_library` via vcpkg + MSVC

Known quirk fixed: `soundtouch.pc` reports its own subdirectory as the include
path, but the codebase uses `#include <soundtouch/SoundTouch.h>`. Fixed by
stepping up to the parent with `get_filename_component(... DIRECTORY)`.

### State Serialization — JUCE ValueTree

`getStateInformation`/`setStateInformation` migrated from kvbuf (C struct
serialization) to `juce::ValueTree` binary format. JSON legacy format is still
detected and read on load (`if (*first == '{' && *last == '}')` → JSON path),
so existing presets continue to work. New saves write binary ValueTree only.

### Obsolete Files Removed

| File | Reason |
|:-----|:-------|
| `JUCE/mx_tune.jucer` | Projucer project file — replaced by `juce_add_plugin()` |
| `JUCE/Source/include_juce_audio_devices.mm` | Standalone app wrapper — format not built |
| `JUCE/Source/include_juce_audio_formats.mm` | Standalone app wrapper — format not built |
| `JUCE/Source/include_juce_audio_utils.mm` | Standalone app wrapper — format not built |
| `JUCE/Source/standalone_Info.plist` | Standalone app bundle config — format not built |
| `src/main.cpp` | LV2 debug test harness — format not built |
| `src/mx_tune_lv2.cpp` | LV2 plugin wrapper — format not built |
| `mx_tune.project` | CodeLite IDE file |
| `mx_tune.workspace` | CodeLite IDE file |
| `mx_tune.ttl` | LV2 metadata — format not built |
| `manifest.ttl` | LV2 metadata — format not built |
| `build_linux.sh` | Manual build script — replaced by CI |
| `build_macos.sh` | Manual build script — replaced by CI |
| `build_windows.sh` | Manual build script — replaced by CI |
| `soundtouch.patch` | Was never applied by anything |

---

## Verification Checklist

**Build:**
- [x] Clean build on macOS (Ninja, Homebrew deps)
- [x] Clean build on Ubuntu (GCC, Ninja, apt deps)
- [x] Clean build on Windows (MSVC, vcpkg deps)
- [x] All existing unit tests pass (`tests/`)

**CI:**
- [x] PR build workflow passes on all three platforms
- [x] Release workflow produces VST3 + AU artifacts

**Runtime (manual):**
- [ ] VST3 loads in Logic Pro — no crash on load/unload
- [ ] AU loads in Logic Pro — processes audio, tuning behaviour unchanged
- [ ] VST3 loads in Reaper — no crash on load/unload
- [ ] No latency regression

> Runtime checks remain open — manual DAW testing to be done with the dev
> tooling setup (Plan 05). Automated CI covers build correctness.

---

## After This Merge

The repo is now on JUCE 8 native CMake. All plans are unblocked:

- **Plan 05 (Dev Tooling)**: `COPY_PLUGIN_AFTER_BUILD` + AudioPluginHost — do immediately
- **Plan 06 (CMake 4 Upgrade)**: One-line minimum bump — do immediately  
- **Plan 02 (ARA)**: Start `feature/ara-phase1` off `master`
- **Plan 04 (UI)**: Can be designed in parallel with ARA
