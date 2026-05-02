#include "reconstruction.h"

#include "fft.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace dsp {

static double bin_frequency(int k, int nfft, double sample_rate) {
    int folded = std::min(k, nfft - k);
    return static_cast<double>(folded) * sample_rate / static_cast<double>(nfft);
}

Signal reconstruct_by_fft_upsampling(const Signal& input, int factor, double cutoff_hz) {
    if (input.samples.empty()) {
        throw std::runtime_error("cannot reconstruct empty signal");
    }
    if (factor < 1) {
        throw std::runtime_error("factor must be at least 1");
    }
    int n = next_power_of_two(static_cast<int>(input.samples.size()));
    int m = n * factor;
    std::vector<cd> spectrum = to_complex(input.samples, n);
    fft(spectrum, false);
    double cutoff = cutoff_hz > 0.0 ? cutoff_hz : input.sample_rate / 2.0;
    for (int k = 0; k < n; ++k) {
        if (bin_frequency(k, n, input.sample_rate) > cutoff) {
            spectrum[k] = cd(0.0, 0.0);
        }
    }
    std::vector<cd> expanded(m, cd(0.0, 0.0));
    for (int k = 0; k <= n / 2; ++k) {
        if (bin_frequency(k, n, input.sample_rate) <= cutoff) {
            expanded[k] = spectrum[k];
        }
    }
    for (int k = n / 2 + 1; k < n; ++k) {
        if (bin_frequency(k, n, input.sample_rate) <= cutoff) {
            int target = m - (n - k);
            expanded[target] = spectrum[k];
        }
    }
    fft(expanded, true);
    Signal output;
    output.sample_rate = input.sample_rate * static_cast<double>(factor);
    output.samples.resize(m);
    for (int i = 0; i < m; ++i) {
        output.samples[i] = expanded[i].real() * static_cast<double>(factor);
    }
    return output;
}

Signal spectral_denoise(const Signal& input, int nfft, double threshold, int keep_top) {
    if (input.samples.empty()) {
        throw std::runtime_error("cannot denoise empty signal");
    }
    nfft = next_power_of_two(std::max<int>(nfft, static_cast<int>(input.samples.size())));
    std::vector<cd> spectrum = to_complex(input.samples, nfft);
    fft(spectrum, false);
    int half = nfft / 2;
    std::vector<std::pair<double, int>> bins;
    bins.reserve(std::max(0, half - 1));
    double max_magnitude = 0.0;
    for (int k = 1; k < half; ++k) {
        double magnitude = std::abs(spectrum[k]);
        max_magnitude = std::max(max_magnitude, magnitude);
        bins.push_back({magnitude, k});
    }
    std::unordered_set<int> keep;
    if (keep_top > 0) {
        std::sort(bins.begin(), bins.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.first > rhs.first;
        });
        int count = std::min<int>(keep_top, static_cast<int>(bins.size()));
        for (int i = 0; i < count; ++i) {
            keep.insert(bins[i].second);
        }
    } else {
        double limit = std::max(0.0, threshold) * max_magnitude;
        for (const auto& [magnitude, k] : bins) {
            if (magnitude >= limit) {
                keep.insert(k);
            }
        }
    }
    for (int k = 1; k < half; ++k) {
        if (!keep.count(k)) {
            spectrum[k] = cd(0.0, 0.0);
            spectrum[nfft - k] = cd(0.0, 0.0);
        }
    }
    fft(spectrum, true);
    Signal output;
    output.sample_rate = input.sample_rate;
    output.samples = real_part(spectrum, input.samples.size());
    return output;
}

}
