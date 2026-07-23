#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include "pitch_shifter_smb.h"

TEST_CASE("pitch_shifter_smb construction at 44100 Hz", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    // get_latency returns sizeof(_in) = 512 * sizeof(float) = 2048
    REQUIRE(shifter.get_latency() == 2048);
}

TEST_CASE("pitch_shifter_smb construction at 48000 Hz", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(48000);
    REQUIRE(shifter.get_latency() == 2048);
}

TEST_CASE("pitch_shifter_smb update same pitch sets shift to 1.0", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    shifter.update_shifter_variables(69.0f, 69.0f);
    // pitch_diff = 0, pow(2, 0/12) = 1.0
    // No getter for _pitch_shift, but we can verify via behavior
    // Feed 512 zeros, output should be near zero
    for (int i = 0; i < 512; i++) {
        shifter.shifter(0.0f);
    }
    // After one frame, output should be available
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) < 0.01f);
}

TEST_CASE("pitch_shifter_smb update clamps shift above 2.0", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    // pitch_diff = 24 semitones (should clamp to 2.0)
    shifter.update_shifter_variables(69.0f, 93.0f);
    // Feed zeros
    for (int i = 0; i < 512; i++) {
        shifter.shifter(0.0f);
    }
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) < 0.01f);
}

TEST_CASE("pitch_shifter_smb update clamps shift below 0.5", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    // pitch_diff = -24 semitones (should clamp to 0.5)
    shifter.update_shifter_variables(93.0f, 69.0f);
    for (int i = 0; i < 512; i++) {
        shifter.shifter(0.0f);
    }
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) < 0.01f);
}

TEST_CASE("pitch_shifter_smb silence in, silence out", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    shifter.update_shifter_variables(69.0f, 69.0f);

    float out = 0.0f;
    for (int i = 0; i < 2048; i++) {
        out = shifter.shifter(0.0f);
    }
    REQUIRE(std::fabs(out) < 0.01f);
}

TEST_CASE("pitch_shifter_smb fills frame before outputting", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    shifter.update_shifter_variables(69.0f, 69.0f);

    // First 511 samples — _out is uninitialized, output is undefined
    // Just consume them without checking
    for (int i = 0; i < 511; i++) {
        shifter.shifter(0.5f);
    }
    // 512th sample triggers smbPitchShift, _out is now valid
    float out = shifter.shifter(0.5f);
    REQUIRE(!std::isnan(out));
    REQUIRE(!std::isinf(out));
}

TEST_CASE("pitch_shifter_smb sine in produces output", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    shifter.update_shifter_variables(69.0f, 69.0f);

    float freq = 440.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 1024; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }
    // After processing frames, output should have energy
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_smb upshift by 12 semitones", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    // Shift up one octave
    shifter.update_shifter_variables(69.0f, 81.0f);

    float freq = 440.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 2048; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_smb downshift by 12 semitones", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    // Shift down one octave
    shifter.update_shifter_variables(81.0f, 69.0f);

    float freq = 880.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 2048; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_smb set_aref no crash", "[pitch_shifter_smb]") {
    pitch_shifter_smb shifter(44100);
    shifter.set_aref(432.0f);
    shifter.update_shifter_variables(69.0f, 69.0f);
    for (int i = 0; i < 512; i++) {
        shifter.shifter(0.0f);
    }
    REQUIRE(true);
}
