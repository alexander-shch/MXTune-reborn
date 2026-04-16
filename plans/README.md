# MXTune — Plans Index

---

## Execution Order

| # | Plan | Branch | Status | Est. Duration |
|:--|:-----|:-------|:-------|:--------------|
| 1 | [JUCE 8 Upgrade](01_JUCE8_UPGRADE.md) | `feature/juce8-upgrade` | Pending | ~1 week |
| 2 | [Dev Tooling](05_DEV_TOOLING.md) | directly to `master` | Pending | ~1 day |
| 3 | [CMake 4 Upgrade](06_CMAKE4_UPGRADE.md) | `feature/cmake4-upgrade` | Pending | ~1 day |
| 4 | [ARA Implementation](02_ARA_IMPLEMENTATION.md) | `feature/ara-phase*` | Pending | ~12 weeks |
| 5 | [ARA Testing](03_ARA_TESTING.md) | runs alongside plan 4 | Pending | ongoing |
| 6 | [UI Modernization](04_UI_MODERNIZATION/04_UI_MODERNIZATION.md) | `feature/ui-modernization` | Pending | ~4 weeks |

> Plans 2 and 3 should be done immediately after plan 1 merges, before any major feature work.
> Plan 6 can be designed in parallel with plans 1–4, but must be implemented after plan 1 is merged.

---

## Plan Summaries

### 01 — JUCE 8 Upgrade
**Target:** Replace JUCE 7.0.5 with JUCE 8.0.12. Build-system change only — no DSP or UI logic touched.

Key changes:
- Update JUCE download URL in both CI workflows
- Remove `VST3_SDK/` directory (JUCE 8 bundles VST3 3.8.0 internally)
- Bump `cmake_minimum_required` from 3.15 → 3.22

**Done when:** Clean three-platform CI build, plugin loads and tunes correctly in Logic Pro and Reaper.

---

### 02 — ARA Implementation
**Target:** Migrate from real-time `processBlock` scanning to full-clip offline analysis via JUCE 8 ARA.
**Prerequisite:** Plan 01 merged to `master`.

| Phase | Goal | Duration |
|:------|:-----|:---------|
| 1 — ARA Handshake | Plugin registers as ARA in Logic Pro / Studio One | ~2 weeks |
| 2 — Offline Scanning | Full-clip pitch data via `ARAAudioSourceReader`, no playback needed | ~3 weeks |
| 3 — Enhanced Metadata | Vibrato extraction and transient detection stored in ARA document model | ~3 weeks |

Key pre-implementation code changes required in `src/`:
- Add `mx_tune::scan()` — detection-only path, no pitch shifting
- `MXTunePlaybackRegionRenderer` owns its own `mx_tune` scan instance; results merged to audio thread via `AsyncUpdater`

Non-ARA hosts (Ableton, FL Studio) fall back to existing `processBlock` path automatically via `isBoundToARA()`.

---

### 03 — ARA Testing
**Target:** Validate correctness and stability of the ARA implementation across DSP edge cases, host behaviours, and UI stress conditions.
**Runs:** Alongside and after Plan 02.

Test categories:
- **DSP unit tests** — sine sweep, static note, noise rejection, sibilance (DAW-independent)
- **ARA source management** — instant load, file swap, concurrency, sample rate mismatch
- **Timeline layout** — item split, crossfade, slip edit, reverse, empty gaps (Reaper gauntlet)
- **Vocal-specific** — vibrato, formant shift, micro-scoops, metal screams
- **UI/UX stress** — zoom, high DPI, undo/redo, offline render, multi-instance torture tests

---

### 06 — CMake 4 Upgrade
**Target:** Bump `cmake_minimum_required` to CMake 4.x before major feature work begins.
**Prerequisite:** Plan 01 merged to `master`.

Key risk: macOS SDK handling changed in 4.0 — no longer sets `-isysroot` automatically. Likely transparent with Apple Clang but must be verified on CI. One-line fix if needed (`-DCMAKE_OSX_SYSROOT=macosx`).

---

### 05 — Dev Tooling
**Target:** Replace the manual build→copy→restart-DAW loop with a one-command dev cycle.
**Prerequisite:** Plan 01 merged to `master`.

Key changes:
- `COPY_PLUGIN_AFTER_BUILD TRUE` in `juce_add_plugin()` — auto-copies plugin after every build
- Build JUCE `AudioPluginHost` from extras — relaunches in ~1s vs Reaper's full scan
- `scripts/dev.sh` — builds and relaunches the host with MXTune pre-loaded

See **[05_DEV_TOOLING.md](05_DEV_TOOLING.md)** for full setup details and a "when to use what" guide.

---

### 04 — UI Modernization
**Target:** Replace the raw-colour JUCE 7 UI with a cohesive design system matching professional DAW plugins (FabFilter, iZotope, Melodyne aesthetic).
**Prerequisite:** Plan 01 merged to `master` (requires JUCE 8 Metal/Direct2D backends).

See **[04_UI_MODERNIZATION/](04_UI_MODERNIZATION/04_UI_MODERNIZATION.md)** for the full spec including reference mockup, colour tokens, typography, layout breakdown, and implementation steps.

Key deliverables:
- `MXTuneTheme.h` — design token constants (colours, spacing, radii)
- `MXTuneLookAndFeel` — custom JUCE LookAndFeel (pill toggles, flat sliders, styled buttons)
- Pitch grid repaint — alternating row shading, blob glow, pitch lines above blobs
- Status bar — live pitch, cents offset, zoom, time position
