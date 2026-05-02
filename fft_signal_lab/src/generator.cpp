#include "generator.h"

#include <cmath>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace dsp {

static const double PI = std::acos(-1.0);

static std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            parts.push_back(item);
        }
    }
    return parts;
}

Signal generate_signal(const std::string& spec, double sample_rate, double duration, unsigned seed) {
    if (sample_rate <= 0.0) {
        throw std::runtime_error("sample rate must be positive");
    }
    if (duration <= 0.0) {
        throw std::runtime_error("duration must be positive");
    }
    int n = std::max(1, static_cast<int>(std::llround(sample_rate * duration)));
    Signal signal;
    signal.sample_rate = sample_rate;
    signal.samples.assign(n, 0.0);
    std::mt19937 rng(seed);
    for (const std::string& token : split(spec, ',')) {
        std::vector<std::string> parts = split(token, ':');
        if (parts.empty()) {
            continue;
        }
        if (parts[0] == "sine" || parts[0] == "sin") {
            if (parts.size() < 3) {
                throw std::runtime_error("sine generator format: sine:freq:amplitude[:phase]");
            }
            double freq = std::stod(parts[1]);
            double amplitude = std::stod(parts[2]);
            double phase = parts.size() >= 4 ? std::stod(parts[3]) : 0.0;
            for (int i = 0; i < n; ++i) {
                double t = static_cast<double>(i) / sample_rate;
                signal.samples[i] += amplitude * std::sin(2.0 * PI * freq * t + phase);
            }
        } else if (parts[0] == "cos" || parts[0] == "cosine") {
            if (parts.size() < 3) {
                throw std::runtime_error("cos generator format: cos:freq:amplitude[:phase]");
            }
            double freq = std::stod(parts[1]);
            double amplitude = std::stod(parts[2]);
            double phase = parts.size() >= 4 ? std::stod(parts[3]) : 0.0;
            for (int i = 0; i < n; ++i) {
                double t = static_cast<double>(i) / sample_rate;
                signal.samples[i] += amplitude * std::cos(2.0 * PI * freq * t + phase);
            }
        } else if (parts[0] == "noise") {
            if (parts.size() < 2) {
                throw std::runtime_error("noise generator format: noise:sigma");
            }
            double sigma = std::stod(parts[1]);
            std::normal_distribution<double> distribution(0.0, sigma);
            for (double& value : signal.samples) {
                value += distribution(rng);
            }
        } else if (parts[0] == "dc") {
            if (parts.size() < 2) {
                throw std::runtime_error("dc generator format: dc:value");
            }
            double value = std::stod(parts[1]);
            for (double& sample : signal.samples) {
                sample += value;
            }
        } else {
            throw std::runtime_error("unknown generator component: " + parts[0]);
        }
    }
    return signal;
}

}
