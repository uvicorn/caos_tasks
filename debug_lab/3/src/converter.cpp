#include "converter.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace telemetry {

Timeline read_timeline(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("unable to open input");
    }

    Timeline timeline{};
    std::string line;
    std::string keyword;
    std::string extra;

    if (!std::getline(input, line)) {
        throw std::runtime_error("missing header");
    }
    
    std::istringstream header(line);
    if (!(header >> keyword >> timeline.end) || keyword != "end" ||
        timeline.end <= 0 || (header >> extra)) {
        throw std::runtime_error("invalid header");
    }

    while (std::getline(input, line)) {
        std::istringstream row(line);
        row >> std::ws;
        if (row.eof()) {
            continue;
        }

        Sample sample{};

        if (!(row >> sample.timestamp >> sample.value) || (row >> extra)) {
            throw std::runtime_error("invalid sample");
        }

        if (sample.timestamp < 0 || sample.timestamp > timeline.end ||
            (!timeline.samples.empty() &&
             sample.timestamp <= timeline.samples.back().timestamp)) {
            throw std::runtime_error("invalid timestamp");
        }

        timeline.samples.push_back(sample);
    }

    if (input.bad()) {
        throw std::runtime_error("unable to read input");
    }
    if (timeline.samples.empty() || timeline.samples.front().timestamp != 0) {
        throw std::runtime_error("missing initial sample");
    }

    return timeline;
}

std::string render_csv(const Timeline& timeline) {
    std::ostringstream output;
    output << "start,end,value\n";

    for (std::size_t index = 0; index < timeline.samples.size(); ++index) {
        const auto& current = timeline.samples.at(index);
        if (current.timestamp == timeline.end) {
            break;
        }
        
        const auto next_timestamp = timeline.samples.at(index + 1).timestamp;
        output << current.timestamp << ',' << next_timestamp << ','
               << current.value << '\n';
    }

    return output.str();
}

}  // namespace telemetry
