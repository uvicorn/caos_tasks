#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace telemetry {

struct Sample {
    std::int64_t timestamp;
    std::int64_t value;
};

struct Timeline {
    std::int64_t end;
    std::vector<Sample> samples;
};

Timeline read_timeline(const std::string& path);
std::string render_csv(const Timeline& timeline);

}  // namespace telemetry
