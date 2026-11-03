#include "converter.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: " << argv[0] << " INPUT\n";
        return 0;
    }
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT\n";
        return 2;
    }

    std::string csv;
    try {
        const auto timeline = telemetry::read_timeline(argv[1]);
        csv = telemetry::render_csv(timeline);
    } catch (...) {
        std::cerr << "Cannot read input\n";
        return 1;
    }

    std::cout << csv;
    if (!std::cout) {
        std::cerr << "Cannot write output\n";
        return 1;
    }
    return 0;
}
