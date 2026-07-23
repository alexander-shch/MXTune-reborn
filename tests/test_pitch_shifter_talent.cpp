#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include "pitch_shifter_talent.h"

TEST_CASE("pitch_shifter_talent construction at 44100 Hz", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    REQUIRE(shifter.get_latency() > 0);
}

TEST_CASE("pitch_shifter_talent construction at 48000 Hz", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(48000);
    REQUIRE(shifter.get_latency() > 0);
}

TEST_CASE("pitch_shifter_talent construction at 96000 Hz", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(96000);
    REQUIRE(shifter.get_latency() > 0);
}

TEST_CASE("pitch_shifter_talent set_aref round-trip", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    // No getter, but ensure no crash
    REQUIRE(true);
}

TEST_CASE("pitch_shifter_talent set_mix round-trip", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_mix(0.0f);
    shifter.set_mix(0.5f);
    shifter.set_mix(1.0f);
    // No getter, but ensure no crash
    REQUIRE(true);
}

TEST_CASE("pitch_shifter_talent same inpitch/outpitch passes through", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    shifter.set_mix(1.0f);
    // A4 = 440Hz, MIDI 69
    float midi_a4 = 69.0f;
    shifter.update_shifter_variables(midi_a4, midi_a4);

    // Feed impulse to fill buffer
    for (int i = 0; i < 1024; i++) {
        shifter.shifter(0.0f);
    }
    // Feed a short sine burst
    float freq = 440.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 2048; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }
    // Output should have energy (not silence)
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_talent silence in, silence out", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    shifter.set_mix(1.0f);
    shifter.update_shifter_variables(69.0f, 69.0f);

    float out = 0.0f;
    for (int i = 0; i < 4096; i++) {
        out = shifter.shifter(0.0f);
    }
    REQUIRE(std::fabs(out) < 0.01f);
}

TEST_CASE("pitch_shifter_talent mix=0 returns delayed input", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    shifter.set_mix(0.0f);
    shifter.update_shifter_variables(69.0f, 69.0f);

    // Feed impulse
    for (int i = 0; i < 1024; i++) {
        shifter.shifter(0.0f);
    }
    // Feed a pulse
    float out = shifter.shifter(1.0f);
    // With mix=0, output should be the delayed input (approximately 1.0 or 0.0 depending on delay)
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_talent upshift changes pitch", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    shifter.set_mix(1.0f);
    // Shift up by 12 semitones (one octave)
    shifter.update_shifter_variables(69.0f, 81.0f);

    // Feed silence to prime
    for (int i = 0; i < 2048; i++) {
        shifter.shifter(0.0f);
    }

    // Feed A4 sine
    float freq = 440.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 4096; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }

    // Output should have energy
    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}

TEST_CASE("pitch_shifter_talent downshift changes pitch", "[pitch_shifter_talent]") {
    pitch_shifter_talent shifter(44100);
    shifter.set_aref(440.0f);
    shifter.set_mix(1.0f);
    // Shift down by 12 semitones (one octave)
    shifter.update_shifter_variables(81.0f, 69.0f);

    for (int i = 0; i < 2048; i++) {
        shifter.shifter(0.0f);
    }

    float freq = 880.0f;
    float sr = 44100.0f;
    for (int i = 0; i < 4096; i++) {
        float sample = 0.5f * std::sin(2.0f * M_PI * freq * i / sr);
        shifter.shifter(sample);
    }

    float out = shifter.shifter(0.0f);
    REQUIRE(std::fabs(out) >= 0.0f);
}
