#pragma once

#include <complex>
#include <vector>

namespace dsp {

using cd = std::complex<double>;

int next_power_of_two(int n);
void fft(std::vector<cd>& a, bool invert);
std::vector<cd> to_complex(const std::vector<double>& samples, int nfft);
std::vector<double> real_part(const std::vector<cd>& values, std::size_t limit);

}
