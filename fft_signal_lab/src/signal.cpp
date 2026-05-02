#include "signal.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace dsp {

static bool parse_number(const std::string& text, double& value) {
    try {
        std::size_t pos = 0;
        value = std::stod(text, &pos);
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
            ++pos;
        }
        return pos == text.size();
    } catch (...) {
        return false;
    }
}

Signal read_csv_signal(const std::string& path, double sample_rate) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("cannot open input file: " + path);
    }
    Signal signal;
    signal.sample_rate = sample_rate;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        for (char& c : line) {
            if (c == ';') {
                c = ',';
            }
        }
        std::stringstream ss(line);
        std::string first;
        std::string second;
        std::getline(ss, first, ',');
        std::getline(ss, second, ',');
        double value = 0.0;
        bool ok = false;
        if (!second.empty()) {
            ok = parse_number(second, value);
        }
        if (!ok) {
            ok = parse_number(first, value);
        }
        if (ok) {
            signal.samples.push_back(value);
        }
    }
    if (signal.samples.empty()) {
        throw std::runtime_error("input signal is empty: " + path);
    }
    return signal;
}

void write_csv_signal(const std::string& path, const Signal& signal) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("cannot open output file: " + path);
    }
    out.setf(std::ios::fixed);
    out.precision(12);
    out << "time,value\n";
    for (std::size_t i = 0; i < signal.samples.size(); ++i) {
        out << static_cast<double>(i) / signal.sample_rate << ',' << signal.samples[i] << '\n';
    }
}

double mean(const std::vector<double>& values) {
    if (values.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    return sum / static_cast<double>(values.size());
}

void remove_mean(std::vector<double>& values) {
    double m = mean(values);
    for (double& value : values) {
        value -= m;
    }
}

double mse(const std::vector<double>& a, const std::vector<double>& b) {
    std::size_t n = std::min(a.size(), b.size());
    if (n == 0) {
        return 0.0;
    }
    double result = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double d = a[i] - b[i];
        result += d * d;
    }
    return result / static_cast<double>(n);
}

std::vector<double> first_n(const std::vector<double>& values, std::size_t n) {
    n = std::min(n, values.size());
    return std::vector<double>(values.begin(), values.begin() + static_cast<std::ptrdiff_t>(n));
}

}
