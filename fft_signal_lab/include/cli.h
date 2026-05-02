#pragma once

#include <string>
#include <vector>

namespace dsp {

int run_cli(int argc, char** argv);

class ArgParser {
public:
    explicit ArgParser(const std::vector<std::string>& args);
    bool has(const std::string& key) const;
    std::string get(const std::string& key, const std::string& fallback = "") const;
    int get_int(const std::string& key, int fallback) const;
    double get_double(const std::string& key, double fallback) const;

private:
    std::vector<std::string> args_;
};

}
