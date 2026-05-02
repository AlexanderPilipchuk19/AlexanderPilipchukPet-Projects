#pragma once

#include <string>
#include <vector>

namespace dsp {

std::vector<double> make_window(const std::string& type, int n);
double window_sum(const std::vector<double>& window);
void apply_window(std::vector<double>& values, const std::vector<double>& window);

}
