#ifndef FILTER_H
#define FILTER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    float x_min, x_max;
    float y_min, y_max;
    float z_min, z_max;
} BoundingBox;

size_t filter_points_naive(
    const float* xs, const float* ys, const float* zs, 
    size_t count, 
    BoundingBox box, 
    uint32_t* out_indices
);

size_t filter_points_simd(
    const float* xs, const float* ys, const float* zs, 
    size_t count, 
    BoundingBox box, 
    uint32_t* out_indices
);

#endif // FILTER_H
