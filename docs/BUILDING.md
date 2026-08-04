# Building MXTune from Source

## macOS

Prerequisites: Homebrew, Xcode command-line tools, CMake, Git.

```bash
./build_macos.sh
```

The script handles framework bundling, rpath fixing, and deep-signing automatically. See [DEVELOPER_NOTES.md](./DEVELOPER_NOTES.md#macos-self-contained-bundles) for details.

## Windows (MSVC + vcpkg)

Prerequisites: [Visual Studio 2022](https://visualstudio.microsoft.com/) (with C++ workload), [vcpkg](https://vcpkg.io/).

### Using vcpkg

1. Install vcpkg and set the `VCPKG_ROOT` environment variable:
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   .\vcpkg\bootstrap-vcpkg.bat
   setx VCPKG_ROOT "C:\path\to\vcpkg"
   ```

2. Install dependencies:
   ```powershell
   vcpkg install fftw3 aubio soundtouch rubberband libsamplerate --triplet x64-windows
   ```

3. Configure with CMake using the vcpkg toolchain file (this is the generator CI builds with):
   ```powershell
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release ^
     "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
     -DVCPKG_TARGET_TRIPLET=x64-windows
   ```

4. Build:
   ```powershell
   cmake --build build
   ```

### Notes

- Dependencies are linked dynamically; the required DLLs are copied into the VST3 bundle's `Contents/x86_64-win` folder automatically as part of the build.
- If `sleef` (a transitive vcpkg dependency) fails to build under Ninja with newer Visual Studio versions (seen with VS2026), configure with the Visual Studio generator instead:
  ```powershell
  cmake -B build -G "Visual Studio 17 2022" -A x64 ^
    "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows
  cmake --build build --config Release
  ```
- The built VST3 bundle will be in `build/MXTune_artefacts/Release/VST3/MXTune.vst3`.

## Linux

Prerequisites: GCC, CMake, pkg-config, and development headers for FFTW, SoundTouch, Aubio, RubberBand, libsamplerate.

```bash
./build_linux.sh
```

The Linux build produces a dynamically linked `.so` inside a standard `.vst3` bundle structure.
