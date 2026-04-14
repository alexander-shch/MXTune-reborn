# Plan 06 — CMake 4 Upgrade

**Branch:** `feature/cmake4-upgrade`
**Prerequisite:** Plan 01 (JUCE 8 Upgrade) merged to `master`
**Priority:** Before Plans 02, 04 — cleanest to upgrade before ARA or UI work adds build complexity

---

## Goal

Upgrade `cmake_minimum_required` from 3.31 → 4.x (latest stable at time of execution).
Ensure all three CI platforms build cleanly under CMake 4 policies.

---

## CMake 4.0 Breaking Changes Relevant to This Project

| Change | Impact |
|:-------|:-------|
| macOS no longer sets `-isysroot` or chooses an SDK by default | May require explicit `-DCMAKE_OSX_SYSROOT=macosx` if the compiler doesn't pick up the SDK on its own |
| `CMAKE_<LANG>_USING_LINKER_MODE` removed, replaced by `CMAKE_<LANG>_LINK_MODE` | Low risk — we don't set this manually |
| Removed compatibility with `cmake_minimum_required(VERSION < 3.5)` | Not relevant — we're at 3.31 |
| VS 14 2015 and VS 15 2017 generators removed | Not relevant — we use VS 17 2022 |

### macOS SDK change (main risk)

CMake 4.0 changed macOS builds to no longer automatically pass `-isysroot` to the compiler.
Apple's Clang picks up the SDK on its own so this should be transparent, but must be verified.
If the macOS CI build fails with missing SDK headers, add `-DCMAKE_OSX_SYSROOT=macosx` to the cmake configure step.

---

## Steps

1. Create branch `feature/cmake4-upgrade` from `master`
2. Bump `cmake_minimum_required(VERSION 3.31)` → `cmake_minimum_required(VERSION 4.x)` in:
   - `CMakeLists.txt`
   - `tests/CMakeLists.txt`
3. Run local macOS build — confirm no SDK or linker errors
4. Push branch and check all three CI platforms pass
5. If macOS fails with SDK errors: add `-DCMAKE_OSX_SYSROOT=macosx` to the macOS cmake configure step in both workflow files
6. Merge to `master`

---

## Done When

Clean three-platform CI build with `cmake_minimum_required(VERSION 4.x)` and no policy warnings.
