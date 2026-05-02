#pragma once

#include <string>
#include <vector>

namespace dsp {

struct Signal {
    double sample_rate = 1.0;
    std::vector<double> samples;
};

Signal read_csv_signal(const std::string& path, double sample_rate);
void write_csv_signal(const std::string& path, const Signal& signal);
double mean(const std::vector<double>& values);
void remove_mean(std::vector<double>& values);
double mse(const std::vector<double>& a, const std::vector<double>& b);
std::vector<double> first_n(const std::vector<double>& values, std::size_t n);

}
