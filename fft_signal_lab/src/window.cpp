#include "window.h"

#include <cmath>
#include <numeric>
#include <stdexcept>

namespace dsp {

static const double PI = std::acos(-1.0);

std::vector<double> make_window(const std::string& type, int n) {
    if (n <= 0) {
        throw std::runtime_error("window size must be positive");
    }
    std::vector<double> w(n, 1.0);
    if (type == "rect" || type == "rectangular" || type.empty()) {
        return w;
    }
    if (n == 1) {
        return w;
    }
    for (int i = 0; i < n; ++i) {
        double x = 2.0 * PI * static_cast<double>(i) / static_cast<double>(n - 1);
        if (type == "hann") {
            w[i] = 0.5 - 0.5 * std::cos(x);
        } else if (type == "hamming") {
            w[i] = 0.54 - 0.46 * std::cos(x);
        } else if (type == "blackman") {
            w[i] = 0.42 - 0.5 * std::cos(x) + 0.08 * std::cos(2.0 * x);
        } else {
            throw std::runtime_error("unknown window type: " + type);
        }
    }
    return w;
}

double window_sum(const std::vector<double>& window) {
    return std::accumulate(window.begin(), window.end(), 0.0);
}

void apply_window(std::vector<double>& values, const std::vector<double>& window) {
    if (values.size() != window.size()) {
        throw std::runtime_error("window size mismatch");
    }
    for (std::size_t i = 0; i < values.size(); ++i) {
        values[i] *= window[i];
    }
}

}
