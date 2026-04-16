cmake_minimum_required(VERSION 3.22)

file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" MXTUNE_VERSION)
string(STRIP "${MXTUNE_VERSION}" MXTUNE_VERSION)
project(MXTune VERSION ${MXTUNE_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party/JUCE)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
if(WIN32)
    # Provided by vcpkg via CMAKE_TOOLCHAIN_FILE
    find_package(FFTW3  CONFIG REQUIRED)
    find_library(FFTW3F_LIB  NAMES fftw3f  REQUIRED)
    find_path   (FFTW3_INCLUDE_DIR  NAMES fftw3.h  REQUIRED)
    find_library(AUBIO_LIB   NAMES aubio   REQUIRED)
    find_path   (AUBIO_INCLUDE_DIR   NAMES aubio/aubio.h  REQUIRED)
    find_library(SOUNDTOUCH_LIB NAMES SoundTouch REQUIRED)
    find_path   (SOUNDTOUCH_INCLUDE_DIR NAMES SoundTouch/SoundTouch.h REQUIRED)
    find_library(RUBBERBAND_LIB NAMES rubberband REQUIRED)
    find_path   (RUBBERBAND_INCLUDE_DIR NAMES rubberband/RubberBandStretcher.h REQUIRED)
    find_package(SampleRate CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFTW3F     REQUIRED IMPORTED_TARGET fftw3f)
    pkg_check_modules(AUBIO      REQUIRED IMPORTED_TARGET aubio)
    pkg_check_modules(SOUNDTOUCH REQUIRED IMPORTED_TARGET soundtouch)
    pkg_check_modules(RUBBERBAND REQUIRED IMPORTED_TARGET rubberband)
    # soundtouch.pc reports the soundtouch/ subdir as its include path, but our
    # code uses #include <soundtouch/SoundTouch.h> — step up to the parent.
    get_filename_component(_soundtouch_inc_parent "${SOUNDTOUCH_INCLUDE_DIRS}" DIRECTORY)
    if(NOT APPLE)
        pkg_check_modules(SAMPLERATE REQUIRED IMPORTED_TARGET samplerate)
    endif()
endif()

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
if(APPLE)
    set(_MXTUNE_FORMATS VST3 AU)
else()
    set(_MXTUNE_FORMATS VST3)
endif()

juce_add_plugin(MXTune
    COMPANY_NAME                "MXTune"
    PLUGIN_MANUFACTURER_CODE    MxTn
    PLUGIN_CODE                 MxTu
    FORMATS                     ${_MXTUNE_FORMATS}
    PRODUCT_NAME                "MXTune"
    DESCRIPTION                 "pitch correction"
    VERSION                     "${MXTUNE_VERSION}"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    VST3_CATEGORIES             "Fx" "Pitch Shift" "Stereo"
    AU_MAIN_TYPE                kAudioUnitType_Effect
    AU_SANDBOX_SAFE             TRUE
)

juce_generate_juce_header(MXTune)

target_sources(MXTune PRIVATE
    src/plugin/KeyDetectGui.cpp
    src/plugin/PluginGui.cpp
    src/plugin/PluginParameter.cpp
    src/plugin/PluginProcessor.cpp
    src/plugin/SettingGui.cpp
    src/auto_tune.cpp
    src/delay.cpp
    src/manual_tune.cpp
    src/midi_msg.cpp
    src/mx_tune.cpp
    src/pitch_detector_aubio.cpp
    src/pitch_detector_talent.cpp
    src/pitch_shifter_rb.cpp
    src/pitch_shifter_smb.cpp
    src/pitch_shifter_st.cpp
    src/pitch_shifter_talent.cpp
    src/ring_buffer.cpp
    src/smbPitchShift.cpp
    src/undo_redo.cpp
)

target_compile_definitions(MXTune
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_WASAPI=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_DISPLAY_SPLASH_SCREEN=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
    PRIVATE
        MXTUNE_VERSION="${MXTUNE_VERSION}"
)

target_include_directories(MXTune PRIVATE src src/plugin)

target_link_libraries(MXTune
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
)

if(WIN32)
    target_include_directories(MXTune PRIVATE
        "${FFTW3_INCLUDE_DIR}"
        "${AUBIO_INCLUDE_DIR}"
        "${SOUNDTOUCH_INCLUDE_DIR}"
        "${RUBBERBAND_INCLUDE_DIR}"
    )
    target_link_libraries(MXTune PRIVATE
        FFTW3::fftw3 "${FFTW3F_LIB}"
        "${AUBIO_LIB}"
        "${SOUNDTOUCH_LIB}"
        "${RUBBERBAND_LIB}"
        SampleRate::samplerate
    )
else()
    target_include_directories(MXTune PRIVATE "${_soundtouch_inc_parent}")
    target_link_libraries(MXTune PRIVATE
        PkgConfig::FFTW3F
        PkgConfig::AUBIO
        PkgConfig::SOUNDTOUCH
        PkgConfig::RUBBERBAND
    )
    if(NOT APPLE)
        target_link_libraries(MXTune PRIVATE PkgConfig::SAMPLERATE)
    endif()
endif()
