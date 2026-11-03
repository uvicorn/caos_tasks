#include "engine.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>
#include <vector>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT_FILE|-\n";
        return 2;
    }

    std::ifstream file;
    std::istream *input = &std::cin;
    if (std::string(argv[1]) != "-") {
        file.open(argv[1]);
        if (!file) {
            std::cerr << "Cannot open input file\n";
            return 1;
        }
        input = &file;
    }

    std::vector<std::int16_t> samples;
    std::string token;
    while (*input >> token) {
        std::int32_t value = 0;
        const auto [end, error] =
            std::from_chars(token.data(), token.data() + token.size(), value);
        if (error != std::errc{} || end != token.data() + token.size() ||
            value < std::numeric_limits<std::int16_t>::min() ||
            value > std::numeric_limits<std::int16_t>::max()) {
            std::cerr << "Invalid sample\n";
            return 1;
        }
        if (samples.size() == max_samples) {
            std::cerr << "Too many samples\n";
            return 1;
        }
        samples.push_back(static_cast<std::int16_t>(value));
    }
    if (!input->eof()) {
        std::cerr << "Cannot read input\n";
        return 1;
    }
    if (samples.empty()) {
        std::cerr << "No samples\n";
        return 1;
    }

    const auto score = telemetry_fold(samples.data(), samples.size());
    std::cout << "samples=" << samples.size() << '\n'
              << "score=" << score << '\n';
    return std::cout ? 0 : 1;
}
