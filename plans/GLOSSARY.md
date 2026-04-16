# MXTune — Technical Glossary

Terms used across the plans in this directory.

---

## Audio Plugin Formats

**VST3** — Virtual Studio Technology 3. The primary cross-platform plugin format
(Steinberg). Supported on Linux, macOS, and Windows. MXTune targets VST3 on all
platforms.

**AU (Audio Units)** — Apple's native plugin format. macOS-only. Required for
Logic Pro hosting. MXTune builds AU alongside VST3 on macOS only.

**LV2** — Linux-only open plugin format. Was partially implemented in the old
codebase (`mx_tune_lv2.cpp`, `*.ttl` metadata). Removed in v2.0.0 — the format
was never built or shipped.

**Standalone** — A standalone application wrapper (not a plugin). JUCE can build
it, but MXTune does not use it. Standalone source files were removed in v2.0.0.

---

## JUCE Concepts

**JUCE** — A C++ framework for audio plugins and applications. MXTune uses JUCE
for its plugin wrapper, GUI, state management, and AU/VST3 export.
Website: https://juce.com

**JUCE native CMake** — The modern way to build JUCE projects: call
`add_subdirectory(third_party/JUCE)` then `juce_add_plugin()`. Replaces the
Projucer workflow. Introduced in JUCE 6, required for JUCE 8 ARA support.

**juce_add_plugin()** — The CMake function that creates the plugin build target.
It handles JuceLibraryCode generation, AU/VST3 wrapper code, plugin metadata
embedding, and artifact paths. Key parameters: `PLUGIN_MANUFACTURER_CODE`,
`PLUGIN_CODE`, `FORMATS`, `AU_MAIN_TYPE`, `AU_SANDBOX_SAFE`.

**juce_generate_juce_header()** — Generates `JuceHeader.h` for the target,
providing the `#include <JuceHeader.h>` entry point. Must be called after
`juce_add_plugin()`.

**Projucer** — JUCE's legacy GUI project manager and code generator. Produces
`CMakeLists.txt` filled with boilerplate. Dropped in MXTune v2.0.0 in favour of
JUCE native CMake.

**JuceLibraryCode** — The set of source files (`include_juce_*.cpp`) that
Projucer generates to pull in JUCE modules. With native CMake, JUCE generates
these automatically — they no longer live in the repo.

**JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR** — A macro that adds a
`LeakDetector` and deletes the copy constructor/assignment operator. Present in
`PluginProcessor.h` as `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutotalentAudioProcessor)`.

**ValueTree** — JUCE's hierarchical, typed data structure. Used for plugin state
serialization (`getStateInformation` / `setStateInformation`). Supports binary
and XML serialization. MXTune migrated from kvbuf to ValueTree in v2.0.0.

**AudioProcessor** — The base class for JUCE plugins. MXTune's implementation is
`AutotalentAudioProcessor` (legacy name from original Autotuner origin).

**AudioProcessorEditor** — The base class for the plugin GUI. Holds a reference
to the processor for parameter reads.

**processBlock()** — The real-time audio callback. Called by the host once per
buffer. In MXTune, this is where pitch detection and shifting runs currently.
ARA (Plan 02) will move offline analysis out of this path.

**prepareToPlay()** — Called by the host before audio processing starts, with
the sample rate and buffer size. MXTune creates its `mx_tune` instance here.

**BusesLayout** — Describes the channel configuration (mono/stereo) a plugin
supports. `isBusesLayoutSupported()` validates configurations the host proposes.

**AudioPluginHost** — A JUCE utility app (in `JUCE/extras/AudioPluginHost`) that
hosts plugins for development. Loads in ~1s vs Reaper's full plugin scan. Used
in the dev tooling setup (Plan 05).

**COPY_PLUGIN_AFTER_BUILD** — A `juce_add_plugin()` parameter that auto-copies
the built plugin to the system plugin folder after each build. Target for Plan 05.

---

## ARA

**ARA (Audio Random Access)** — An extension protocol on top of VST3/AU that
allows a plugin to access the full audio clip offline, not just the current
buffer. Enables full-clip pitch analysis without playback.
Supported hosts: Logic Pro, Studio One, Cubase, Reaper (with ARA extension).

**ARA Document** — The ARA object model managed by the host. Contains audio
sources, musical context, and renderer assignments. Persists across sessions.

**ARAAudioSource** — Represents a single audio clip in the ARA document.
Provides access to the full audio content for offline processing.

**ARAAudioSourceReader** — JUCE's wrapper for reading samples from an
`ARAAudioSource`. Used in Plan 02 to feed the pitch detector the full clip.

**ARAPlaybackRegion** — A time-mapped region that links a portion of an audio
source to a position in the timeline. The renderer operates on playback regions.

**MXTunePlaybackRegionRenderer** — The ARA renderer class to be implemented in
Plan 02. It will own an `mx_tune` scan instance and merge results to the audio
thread via `AsyncUpdater`.

**isBoundToARA()** — JUCE method that returns true when the plugin instance is
hosted in an ARA-aware host. Non-ARA hosts (Ableton, FL Studio) fall back to
the existing `processBlock` path automatically.

**ARA Handshake** — Phase 1 of Plan 02: registering the plugin as ARA-capable
so Logic Pro / Studio One recognise it as an ARA plugin.

---

## Build System

**CMake** — The build system used by MXTune. Minimum version: 3.22 (JUCE 8
requirement). Plan 06 will bump the minimum to CMake 4.x.

**Ninja** — A fast build executor used with CMake on Linux and macOS
(`-G Ninja`). Not used on Windows (Visual Studio generator used instead).

**pkg-config** — Linux/macOS tool for querying library compile and link flags.
Used in MXTune's CMakeLists.txt for FFTW3, aubio, soundtouch, rubberband, and
libsamplerate on non-Windows platforms.

**vcpkg** — Microsoft's C++ package manager. Used for Windows builds to provide
fftw3, aubio, soundtouch, rubberband, and libsamplerate via the vcpkg toolchain.

**PkgConfig::FFTW3F** — The CMake imported target created by
`pkg_check_modules(FFTW3F ...)`. Provides compile flags and link libraries for
the single-precision FFTW3 library.

**juce_recommended_config_flags** — A JUCE-provided CMake interface target that
applies optimisation flags appropriate for the build type. Linked `PUBLIC` so
that flags propagate to consumers.

**ccache** — A compiler cache that speeds up rebuilds. Applied on Linux/macOS CI
via `CMAKE_CXX_COMPILER_LAUNCHER=ccache`.

**VCPKG_INSTALLATION_ROOT** — Environment variable set by GitHub Actions Windows
runners pointing to the vcpkg installation directory.

---

## DSP Libraries

**FFTW3** — Fastest Fourier Transform in the West. Used for FFT-based pitch
detection and pitch shifting (SMB phase vocoder). MXTune uses the single-
precision variant (`fftw3f`).

**aubio** — Audio analysis library. Provides the `pitch_detector_aubio`
implementation (`src/pitch_detector_aubio.cpp`).

**SoundTouch** — Tempo and pitch processing library. Provides the
`pitch_shifter_st` implementation (`src/pitch_shifter_st.cpp`).

**RubberBand** — High-quality time-stretching and pitch-shifting library.
Provides the `pitch_shifter_rb` implementation (`src/pitch_shifter_rb.cpp`).

**libsamplerate** — Sample rate conversion library. Used on Linux (and Windows
via vcpkg). Not needed on macOS (Core Audio handles resampling).

**SMB Phase Vocoder** — A pitch-shifting algorithm implemented in
`src/smbPitchShift.cpp` / `src/pitch_shifter_smb.cpp`. Classic FFT-based
approach.

---

## MXTune Internals

**mx_tune** — The core C++ class (`src/mx_tune.h`, `src/mx_tune.cpp`).
Orchestrates pitch detection and shifting. Owned by `AutotalentAudioProcessor`.

**manual_tune** — Stores manual pitch correction data entered by the user
(`src/manual_tune.h`, `src/manual_tune.cpp`).

**auto_tune** — Handles automatic pitch correction logic
(`src/auto_tune.cpp`).

**kvbuf** — The legacy C struct-based state serialisation format used before
v2.0.0. Still read on load for backwards compatibility (detected by JSON
braces `{ ... }`). New saves use ValueTree binary format.

**mx_tune::scan()** — A detection-only path planned for Plan 02 (ARA). Will run
pitch detection without triggering pitch shifting, used for offline pre-analysis.

**AsyncUpdater** — JUCE class for safely posting a callback from a non-UI thread
to the message thread. Planned for Plan 02 to merge ARA scan results to the
audio thread.

---

## CI / Release

**pr-build.yml** — GitHub Actions workflow that runs on every pull request.
Builds on Linux, macOS, and Windows; runs unit tests on Ubuntu.

**release.yml** — GitHub Actions workflow that triggers on version tags
(`v*.*.*`). Builds on all three platforms and packages artifacts:
VST3 (all) + AU (macOS). macOS artifacts are ad-hoc signed with `codesign`.

**FORCE_JAVASCRIPT_ACTIONS_TO_NODE24** — Environment variable set in CI workflows
to ensure GitHub Actions JavaScript actions use Node.js 24.

**concurrency.cancel-in-progress** — CI setting that cancels an in-progress run
when a new commit is pushed to the same PR, saving runner minutes.

---

## Versioning

**VERSION file** — Plain text file at the repo root containing the current
version string (e.g. `2.0.0`). Read by `CMakeLists.txt` via `file(READ ...)`.

**v2.0.0** — The version released with the JUCE 8 upgrade. Major bump reflects
the breaking build-system change (Projucer removed, state format changed).

---

## Plans

| ID | File | Subject |
|:---|:-----|:--------|
| 01 | `01_JUCE8_UPGRADE.md` | JUCE 8 migration — **Done** |
| 02 | `02_ARA_IMPLEMENTATION.md` | ARA offline analysis |
| 03 | `03_ARA_TESTING.md` | ARA test suite |
| 04 | `04_UI_MODERNIZATION/` | Design system + UI rewrite |
| 05 | `05_DEV_TOOLING.md` | Fast dev cycle (AudioPluginHost) |
| 06 | `06_CMAKE4_UPGRADE.md` | CMake 4.x minimum bump |
