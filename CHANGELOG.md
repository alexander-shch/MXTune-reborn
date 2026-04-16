# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2026-04-16

_No significant changes documented._

## [1.2.32] - 2026-04-08

### Other
- Clean up ARA testing cases document
- Create ARA_TESTING_CASES.md for MXTune validation

## [1.2.31] - 2026-04-08

### Other
- Create ARA_TESTING_CASES.md for MXTune validation

## [1.2.30] - 2026-04-08

### Added
- Add horizontal and vertical scroll bars to UI (feat)

## [1.2.29] - 2026-04-08

### Added
- Note blob UI, resizable window, macOS standalone app, AU format (feat)

### Other
- Add ccache to all platforms to speed up PR builds (ci)
- Add unit tests for manual_tune and auto_tune (test)
- Add PR build workflow with unit tests and compile checks (ci)

## [1.2.28] - 2026-04-08

### Other
- Fix yaml syntax indentation in release workflow (ci)
- Automate GitHub Release notes from CHANGELOG (ci)

## [1.2.27] - 2026-04-08

### Other
- Automate CHANGELOG.md updates during release (ci)

## [1.2.23] - 2026-04-08

### Added
- **Self-contained macOS bundles**: Dependencies (`rubberband`, `sound-touch`, `aubio`, `fftw`) are now bundled inside the `.vst3` and `.component` bundles.
- **Improved Installation**: Plugins no longer require Homebrew or external libraries to be installed on the user's system.

### Fixed
- **Library Loading Error**: Resolved "Library not loaded" errors on macOS when the plugin was used on systems without Homebrew.
- **Audio Unit Validation**: Fixed AMFI code signature failures during `auval` validation by correctly signing internal dylibs and the main bundle.
- **VST3 Installation**: Fixed a bug where the VST3 bundle was not being correctly created or installed to `/Library/Audio/Plug-Ins/VST3/`.

### Changed
- **Build Script**: `build_macos.sh` now automates dependency bundling, RPath fixing, and deep codesigning.
- **JUCE Patch**: Applied compatibility guard for `CGWindowListCreateImage` on macOS 15.0+ to prevent crashes during GUI initialization.

---

## [1.2.22] - 2026-04-07

### Added
- **Audio Unit (AU) Support**: Added support for the AU plugin format on macOS.
- **macOS Build Automation**: New `build_macos.sh` script for unified building and installation.
- **GitHub Actions**: Automated release workflow for macOS, Windows, and Linux.
