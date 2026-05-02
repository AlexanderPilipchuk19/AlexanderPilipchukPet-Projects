#include "spectrum.h"

#include "fft.h"
#include "window.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace dsp {

SpectrumAnalysis analyze_spectrum(const Signal& signal, int nfft, const std::string& window_type, bool subtract_mean) {
    if (signal.samples.empty()) {
        throw std::runtime_error("cannot analyze empty signal");
    }
    nfft = next_power_of_two(nfft);
    int frame_size = std::min<int>(nfft, static_cast<int>(signal.samples.size()));
    std::vector<double> frame(signal.samples.begin(), signal.samples.begin() + frame_size);
    if (subtract_mean) {
        remove_mean(frame);
    }
    std::vector<double> window = make_window(window_type, frame_size);
    double coherent_gain = std::max(1e-12, window_sum(window));
    apply_window(frame, window);
    std::vector<cd> data = to_complex(frame, nfft);
    fft(data, false);
    int half = nfft / 2;
    SpectrumAnalysis analysis;
    analysis.nfft = nfft;
    analysis.sample_rate = signal.sample_rate;
    analysis.magnitudes.assign(half + 1, 0.0);
    analysis.powers.assign(half + 1, 0.0);
    analysis.phases.assign(half + 1, 0.0);
    for (int k = 0; k <= half; ++k) {
        double scale = (k == 0 || k == half) ? 1.0 : 2.0;
        double magnitude = scale * std::abs(data[k]) / coherent_gain;
        analysis.magnitudes[k] = magnitude;
        analysis.powers[k] = magnitude * magnitude;
        analysis.phases[k] = std::atan2(data[k].imag(), data[k].real());
    }
    return analysis;
}

std::vector<SpectrumBin> find_peaks(const SpectrumAnalysis& analysis, int top_k, double relative_floor) {
    if (top_k <= 0) {
        return {};
    }
    const std::vector<double>& m = analysis.magnitudes;
    if (m.size() < 3) {
        return {};
    }
    double max_value = *std::max_element(m.begin() + 1, m.end());
    double floor_value = std::max(0.0, relative_floor) * max_value;
    std::vector<SpectrumBin> peaks;
    for (int k = 1; k + 1 < static_cast<int>(m.size()); ++k) {
        if (m[k] < floor_value) {
            continue;
        }
        if (m[k] >= m[k - 1] && m[k] >= m[k + 1]) {
            double left = m[k - 1];
            double center = m[k];
            double right = m[k + 1];
            double denominator = left - 2.0 * center + right;
            double delta = 0.0;
            if (std::abs(denominator) > 1e-18) {
                delta = 0.5 * (left - right) / denominator;
                delta = std::clamp(delta, -0.5, 0.5);
            }
            double refined_magnitude = center - 0.25 * (left - right) * delta;
            SpectrumBin bin;
            bin.index = k;
            bin.frequency = (static_cast<double>(k) + delta) * analysis.sample_rate / static_cast<double>(analysis.nfft);
            bin.magnitude = refined_magnitude;
            bin.phase = analysis.phases[k];
            peaks.push_back(bin);
        }
    }
    std::sort(peaks.begin(), peaks.end(), [](const SpectrumBin& lhs, const SpectrumBin& rhs) {
        return lhs.magnitude > rhs.magnitude;
    });
    if (static_cast<int>(peaks.size()) > top_k) {
        peaks.resize(top_k);
    }
    return peaks;
}

void print_peaks(const std::vector<SpectrumBin>& peaks) {
    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(6);
    std::cout << "rank,frequency_hz,magnitude,phase_rad\n";
    for (std::size_t i = 0; i < peaks.size(); ++i) {
        std::cout << i + 1 << ',' << peaks[i].frequency << ',' << peaks[i].magnitude << ',' << peaks[i].phase << '\n';
    }
}

}
