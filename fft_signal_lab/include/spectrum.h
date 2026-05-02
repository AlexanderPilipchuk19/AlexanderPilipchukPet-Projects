#pragma once

#include "signal.h"

#include <string>
#include <vector>

namespace dsp {

struct SpectrumBin {
    int index = 0;
    double frequency = 0.0;
    double magnitude = 0.0;
    double phase = 0.0;
};

struct SpectrumAnalysis {
    int nfft = 0;
    double sample_rate = 0.0;
    std::vector<double> magnitudes;
    std::vector<double> powers;
    std::vector<double> phases;
};

SpectrumAnalysis analyze_spectrum(const Signal& signal, int nfft, const std::string& window_type, bool subtract_mean);
std::vector<SpectrumBin> find_peaks(const SpectrumAnalysis& analysis, int top_k, double relative_floor);
void print_peaks(const std::vector<SpectrumBin>& peaks);

}
