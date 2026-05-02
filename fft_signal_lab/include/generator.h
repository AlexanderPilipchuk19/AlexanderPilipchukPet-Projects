#pragma once

#include "signal.h"

#include <string>

namespace dsp {

Signal generate_signal(const std::string& spec, double sample_rate, double duration, unsigned seed);

}
