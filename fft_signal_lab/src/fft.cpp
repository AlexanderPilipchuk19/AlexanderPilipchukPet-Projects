#include "fft.h"

#include <cmath>
#include <stdexcept>

namespace dsp {

const double PI = std::acos(-1.0);

int next_power_of_two(int n) {
    if (n <= 1) {
        return 1;
    }
    int result = 1;
    while (result < n) {
        result <<= 1;
    }
    return result;
}

void fft(std::vector<cd>& a, bool invert) {
    int n = static_cast<int>(a.size());
    if (n == 0 || (n & (n - 1)) != 0) {
        throw std::runtime_error("fft size must be a positive power of two");
    }
    int k = 0;
    while ((1 << k) < n) k++;
    std::vector<int> rev(n);
    rev[0] = 0;
    int high1 = -1;
    for (int i = 1; i < n; i++) {
        if ((i & (i - 1)) == 0)
            high1++;
        rev[i] = rev[i ^ (1 << high1)];
        rev[i] |= (1 << (k - high1 - 1));
    }

    std::vector<cd> roots(n);
    for (int i = 0; i < n; i++) {
        double alpha = 2 * PI * i / n * (invert ? -1 : 1);
        roots[i] = cd(std::cos(alpha), std::sin(alpha));
    }

    std::vector<cd> cur(n);
    for (int i = 0; i < n; i++)
        cur[i] = a[rev[i]];

    for (int len = 1; len < n; len <<= 1) {
        std::vector<cd> ncur(n);
        int rstep = static_cast<int>(roots.size()) / (len * 2);
        for (int pdest = 0; pdest < n;) {
            int p1 = pdest;
            for (int i = 0; i < len; i++) {
                cd val = roots[i * rstep] * cur[p1 + len];
                ncur[pdest] = cur[p1] + val;
                ncur[pdest + len] = cur[p1] - val;
                pdest++, p1++;
            }
            pdest += len;
        }
        cur.swap(ncur);
    }

    a.swap(cur);

    if (invert) {
        for (cd& x : a)
            x /= n;
    }
}

std::vector<cd> to_complex(const std::vector<double>& samples, int nfft) {
    if (nfft <= 0) {
        throw std::runtime_error("nfft must be positive");
    }
    std::vector<cd> result(nfft, cd(0.0, 0.0));
    int n = std::min<int>(nfft, static_cast<int>(samples.size()));
    for (int i = 0; i < n; ++i) {
        result[i] = cd(samples[i], 0.0);
    }
    return result;
}

std::vector<double> real_part(const std::vector<cd>& values, std::size_t limit) {
    limit = std::min(limit, values.size());
    std::vector<double> result(limit);
    for (std::size_t i = 0; i < limit; ++i) {
        result[i] = values[i].real();
    }
    return result;
}

}
