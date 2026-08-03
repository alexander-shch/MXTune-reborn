# Developer Notes

## Architecture & Dependencies

MXTune is built on **JUCE 7** (upgrading to JUCE 8 — see [plans/](../plans/README.md)) and **CMake**. It bridges legacy C-based pitch algorithms with modern C++ plugin wrappers.

The plugin relies on several high-performance audio libraries:

| Library | Role |
|:--------|:-----|
| **RubberBand** | High-quality time-stretching and pitch-shifting |
| **SoundTouch** | Alternative pitch-shifting engine |
| **Aubio** | Real-time pitch detection (YIN/FFT based) |
| **FFTW3** | Fast Fourier transforms for spectral processing |
| **libsamplerate** | Audio resampling |

---

## Platform-Specific Details

### macOS (Self-Contained Bundles)

Standard macOS plugins dynamically linked to Homebrew libraries crash on machines without Homebrew. `build_macos.sh` handles this in three steps:

1. **Framework Bundling** — copies `.dylib` files from `/opt/homebrew/` into the plugin's `Contents/Frameworks/` directory.
2. **RPath Fixing** — uses `install_name_tool` to rewrite load paths to `@loader_path/../Frameworks/`.
3. **Deep Signing** — signs internal dylibs individually before performing a deep signature on the outer bundle.

### Windows (Dynamic Linking with vcpkg)

Dependencies are dynamically linked using vcpkg with the MSVC toolchain. The required DLLs are automatically copied to the VST3 bundle directory during the build process. This approach provides better compatibility with modern Windows development workflows while still ensuring the plugin is self-contained.

### Linux

Linux builds produce a standard `.so` binary inside a `.vst3` bundle directory structure per the VST3 SDK spec. Runtime dependencies (FFTW3, SoundTouch, etc.) must be installed on the target machine.

---

## JUCE & macOS 15 Compatibility

JUCE 7.0.5 has a known issue with `CGWindowListCreateImage` on macOS 15 Sequoia. The CI build and `build_macos.sh` include a Python patcher that automatically guards this call in the JUCE source before compiling.

---

## CI / Release

- **PR builds** — run on every pull request: unit tests + compile check on all three platforms. See `.github/workflows/pr-build.yml`.
- **Releases** — triggered by pushing a `v*` tag (e.g. `v1.2.29`). Builds all platforms, generates changelog from git history since the last tag, and publishes a GitHub Release. See `.github/workflows/release.yml`.

To publish a release:
```bash
echo "1.2.29" > VERSION
git add VERSION
git commit -m "chore: bump version to 1.2.29"
git tag v1.2.29
git push origin master --tags
```
