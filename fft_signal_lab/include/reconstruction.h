#pragma once

#include "signal.h"

#include <string>

namespace dsp {

Signal reconstruct_by_fft_upsampling(const Signal& input, int factor, double cutoff_hz);
Signal spectral_denoise(const Signal& input, int nfft, double threshold, int keep_top);

}
