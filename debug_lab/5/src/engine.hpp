#pragma once

#include <cstddef>
#include <cstdint>

inline constexpr std::size_t max_samples = 4096;

extern "C" std::int32_t telemetry_fold(const std::int16_t *samples,
                                      std::size_t count) noexcept;
