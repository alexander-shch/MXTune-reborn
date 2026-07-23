#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include "SvfLinearTrapOptimised2.hpp"

TEST_CASE("SvfLinearTrapOptimised2 default construction resets state", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    // tick with zero should return zero
    REQUIRE(filter.tick(0.0) == Catch::Approx(0.0).margin(0.0001));
}

TEST_CASE("SvfLinearTrapOptimised2 resetState clears state", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::LOW_PASS_FILTER, 44100.0);
    // Process some audio to build state
    for (int i = 0; i < 100; i++) {
        filter.tick(1.0);
    }
    filter.resetState();
    // After reset, output should be near zero for DC input
    double out = filter.tick(0.0);
    REQUIRE(std::fabs(out) < 0.01);
}

TEST_CASE("SvfLinearTrapOptimised2 low-pass passes DC", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::LOW_PASS_FILTER, 44100.0);

    // Feed DC (1.0) for enough samples to settle
    double out = 0.0;
    for (int i = 0; i < 1000; i++) {
        out = filter.tick(1.0);
    }
    // Low-pass at 1kHz should pass DC
    REQUIRE(out > 0.9);
}

TEST_CASE("SvfLinearTrapOptimised2 low-pass attenuates high frequency", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(100.0, 0.5, SvfLinearTrapOptimised2::LOW_PASS_FILTER, 44100.0);

    // Feed 10kHz sine (well above 100Hz cutoff)
    double freq = 10000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    // Should be heavily attenuated
    REQUIRE(max_out < 0.1);
}

TEST_CASE("SvfLinearTrapOptimised2 high-pass attenuates DC", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, 44100.0);

    // Feed DC (1.0)
    double out = 0.0;
    for (int i = 0; i < 1000; i++) {
        out = filter.tick(1.0);
    }
    // High-pass should attenuate DC
    REQUIRE(std::fabs(out) < 0.1);
}

TEST_CASE("SvfLinearTrapOptimised2 high-pass passes high frequency", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, 44100.0);

    // Feed 5kHz sine (well above 1kHz cutoff)
    double freq = 5000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    REQUIRE(max_out > 0.5);
}

TEST_CASE("SvfLinearTrapOptimised2 band-pass passes center frequency", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::BAND_PASS_FILTER, 44100.0);

    // Feed 1kHz sine (at center frequency)
    double freq = 1000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    REQUIRE(max_out > 0.3);
}

TEST_CASE("SvfLinearTrapOptimised2 band-pass attenuates DC", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::BAND_PASS_FILTER, 44100.0);

    double out = 0.0;
    for (int i = 0; i < 1000; i++) {
        out = filter.tick(1.0);
    }
    REQUIRE(std::fabs(out) < 0.1);
}

TEST_CASE("SvfLinearTrapOptimised2 notch rejects center frequency", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::NOTCH_FILTER, 44100.0);

    // Feed 1kHz sine (at center frequency)
    double freq = 1000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    // Notch should attenuate center frequency
    REQUIRE(max_out < 0.3);
}

TEST_CASE("SvfLinearTrapOptimised2 all-pass preserves magnitude", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::ALL_PASS_FILTER, 44100.0);

    // Feed 1kHz sine
    double freq = 1000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    // All-pass should preserve magnitude
    REQUIRE(max_out > 0.8);
}

TEST_CASE("SvfLinearTrapOptimised2 bell filter with gain boost", "[svf_filter]") {
    SvfLinearTrapOptimised2 filter;
    filter.setGain(12.0); // +12dB boost
    filter.updateCoefficients(1000.0, 0.5, SvfLinearTrapOptimised2::BELL_FILTER, 44100.0);

    // Feed 1kHz sine (at center frequency)
    double freq = 1000.0;
    double sr = 44100.0;
    double max_out = 0.0;
    for (int i = 0; i < 2000; i++) {
        double sample = std::sin(2.0 * M_PI * freq * i / sr);
        double out = filter.tick(sample);
        if (i > 500 && std::fabs(out) > max_out) {
            max_out = std::fabs(out);
        }
    }
    // Bell filter with +12dB should boost
    REQUIRE(max_out > 0.5);
}
