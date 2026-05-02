#include "cli.h"

#include "fft.h"
#include "generator.h"
#include "reconstruction.h"
#include "signal.h"
#include "spectrum.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace dsp {

ArgParser::ArgParser(const std::vector<std::string>& args) : args_(args) {}

bool ArgParser::has(const std::string& key) const {
    return std::find(args_.begin(), args_.end(), key) != args_.end();
}

std::string ArgParser::get(const std::string& key, const std::string& fallback) const {
    for (std::size_t i = 0; i + 1 < args_.size(); ++i) {
        if (args_[i] == key) {
            return args_[i + 1];
        }
    }
    return fallback;
}

int ArgParser::get_int(const std::string& key, int fallback) const {
    std::string value = get(key, "");
    return value.empty() ? fallback : std::stoi(value);
}

double ArgParser::get_double(const std::string& key, double fallback) const {
    std::string value = get(key, "");
    return value.empty() ? fallback : std::stod(value);
}

static Signal load_or_generate(const ArgParser& args) {
    double fs = args.get_double("--fs", 8000.0);
    if (args.has("--generate")) {
        double duration = args.get_double("--duration", 1.0);
        unsigned seed = static_cast<unsigned>(args.get_int("--seed", 42));
        return generate_signal(args.get("--generate"), fs, duration, seed);
    }
    std::string input = args.get("--input", "");
    if (input.empty()) {
        throw std::runtime_error("expected --input or --generate");
    }
    return read_csv_signal(input, fs);
}

static int run_detect(const ArgParser& args) {
    Signal signal = load_or_generate(args);
    int nfft = args.get_int("--nfft", next_power_of_two(static_cast<int>(signal.samples.size())));
    std::string window = args.get("--window", "hann");
    int top = args.get_int("--top", 5);
    double floor = args.get_double("--floor", 0.01);
    SpectrumAnalysis analysis = analyze_spectrum(signal, nfft, window, true);
    std::vector<SpectrumBin> peaks = find_peaks(analysis, top, floor);
    print_peaks(peaks);
    return 0;
}

static int run_reconstruct(const ArgParser& args) {
    Signal signal = load_or_generate(args);
    int factor = args.get_int("--factor", 4);
    double cutoff = args.get_double("--cutoff", signal.sample_rate / 2.0);
    std::string output = args.get("--output", "reconstructed.csv");
    Signal reconstructed = reconstruct_by_fft_upsampling(signal, factor, cutoff);
    write_csv_signal(output, reconstructed);
    std::cout << "wrote " << output << "\n";
    std::cout << "input_samples=" << signal.samples.size() << "\n";
    std::cout << "output_samples=" << reconstructed.samples.size() << "\n";
    std::cout << "output_sample_rate=" << reconstructed.sample_rate << "\n";
    return 0;
}

static int run_denoise(const ArgParser& args) {
    Signal signal = load_or_generate(args);
    int nfft = args.get_int("--nfft", next_power_of_two(static_cast<int>(signal.samples.size())));
    double threshold = args.get_double("--threshold", 0.05);
    int keep_top = args.get_int("--keep-top", 0);
    std::string output = args.get("--output", "clean.csv");
    Signal clean = spectral_denoise(signal, nfft, threshold, keep_top);
    write_csv_signal(output, clean);
    std::cout << "wrote " << output << "\n";
    return 0;
}

static void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("self-test failed: " + message);
    }
}

static int run_self_test() {
    Signal signal = generate_signal("sine:440:1.0,sine:880:0.4,noise:0.01", 8192.0, 1.0, 7);
    SpectrumAnalysis analysis = analyze_spectrum(signal, 8192, "hann", true);
    std::vector<SpectrumBin> peaks = find_peaks(analysis, 5, 0.02);
    bool has440 = false;
    bool has880 = false;
    for (const SpectrumBin& peak : peaks) {
        if (std::abs(peak.frequency - 440.0) < 1.0) {
            has440 = true;
        }
        if (std::abs(peak.frequency - 880.0) < 1.0) {
            has880 = true;
        }
    }
    require(has440, "440 Hz was not detected");
    require(has880, "880 Hz was not detected");

    Signal coarse = generate_signal("sine:32:1.0,sine:64:0.5", 1024.0, 1.0, 1);
    Signal reconstructed = reconstruct_by_fft_upsampling(coarse, 4, 100.0);
    Signal dense = generate_signal("sine:32:1.0,sine:64:0.5", 4096.0, 1.0, 1);
    double reconstruction_mse = mse(reconstructed.samples, dense.samples);
    require(reconstruction_mse < 1e-20, "reconstruction mse is too high");

    Signal clean = generate_signal("sine:440:1.0,sine:880:0.5", 8192.0, 1.0, 1);
    Signal noisy = generate_signal("sine:440:1.0,sine:880:0.5,noise:0.4", 8192.0, 1.0, 3);
    Signal denoised = spectral_denoise(noisy, 8192, 0.0, 2);
    double noisy_mse = mse(noisy.samples, clean.samples);
    double denoised_mse = mse(denoised.samples, clean.samples);
    require(denoised_mse * 1000.0 < noisy_mse, "denoise did not improve mse enough");

    std::cout << "smoke tests passed\n";
    std::cout << "reconstruction_mse=" << reconstruction_mse << "\n";
    std::cout << "noisy_mse=" << noisy_mse << "\n";
    std::cout << "denoised_mse=" << denoised_mse << "\n";
    return 0;
}

static void print_help() {
    std::cout << "FFT Signal Lab\n";
    std::cout << "modes:\n";
    std::cout << "  detect --input signal.csv --fs 8000 --nfft 2048 --window hann --top 5\n";
    std::cout << "  reconstruct --input sampled.csv --fs 1000 --factor 8 --cutoff 180 --output reconstructed.csv\n";
    std::cout << "  denoise --input noisy.csv --fs 8000 --threshold 0.05 --output clean.csv\n";
    std::cout << "  self-test\n";
}

int run_cli(int argc, char** argv) {
    try {
        if (argc < 2) {
            print_help();
            return 1;
        }
        std::vector<std::string> raw;
        for (int i = 2; i < argc; ++i) {
            raw.emplace_back(argv[i]);
        }
        std::string mode = argv[1];
        ArgParser args(raw);
        if (mode == "detect") {
            return run_detect(args);
        }
        if (mode == "reconstruct") {
            return run_reconstruct(args);
        }
        if (mode == "denoise") {
            return run_denoise(args);
        }
        if (mode == "self-test" || mode == "test") {
            return run_self_test();
        }
        if (mode == "help" || mode == "--help" || mode == "-h") {
            print_help();
            return 0;
        }
        throw std::runtime_error("unknown mode: " + mode);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}

}
