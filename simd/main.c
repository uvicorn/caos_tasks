#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

size_t filter_points_naive(
    const float* xs, const float* ys, const float* zs, 
    size_t count, BoundingBox box, uint32_t* out_indices) 
{
    size_t out_count = 0;
    for (size_t i = 0; i < count; i++) {
        if (xs[i] >= box.x_min && xs[i] <= box.x_max &&
            ys[i] >= box.y_min && ys[i] <= box.y_max &&
            zs[i] >= box.z_min && zs[i] <= box.z_max) {
            out_indices[out_count++] = i;
        }
    }
    return out_count;
}

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

float rand_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

int main() {
    printf("[*] Starting SIMD LiDAR Filter Test...\n");

    const size_t COUNT = 30 * 1024 * 1024;
    
    float* xs = aligned_alloc(32, COUNT * sizeof(float));
    float* ys = aligned_alloc(32, COUNT * sizeof(float));
    float* zs = aligned_alloc(32, COUNT * sizeof(float));
    uint32_t* out_naive = aligned_alloc(32, COUNT * sizeof(uint32_t));
    uint32_t* out_simd = aligned_alloc(32, COUNT * sizeof(uint32_t));

    assert(xs && ys && zs && out_naive && out_simd);

    printf("[+] Allocating memory and generating points...\n");
    srand(time(0));
    for (size_t i = 0; i < COUNT; i++) {
        xs[i] = rand_float(-100.0f, 100.0f);
        ys[i] = rand_float(-100.0f, 100.0f);
        zs[i] = rand_float(-10.0f, 10.0f);
    }

    BoundingBox box = {
        .x_min = -10.0f, .x_max = 10.0f,
        .y_min = 0.0f,   .y_max = 50.0f,
        .z_min = -2.0f,  .z_max = 3.0f
    };

    printf("[1] Running Naive version...\n");
    uint64_t t1 = get_time_ns();
    size_t count_naive = filter_points_naive(xs, ys, zs, COUNT, box, out_naive);

    uint64_t t2 = get_time_ns();
    double time_naive_ms = (t2 - t1) / 1000000.0;

    printf("[2] Running SIMD version...\n");
    uint64_t t3 = get_time_ns();
    size_t count_simd = filter_points_simd(xs, ys, zs, COUNT, box, out_simd);
    uint64_t t4 = get_time_ns();
    double time_simd_ms = (t4 - t3) / 1000000.0;

    printf("\n--- RESULTS ---\n");
    printf("Points found: %zu (Naive), %zu (SIMD)\n", count_naive, count_simd);
    
    if (count_naive != count_simd) {
        fprintf(stderr, "FATAL ERROR: Output counts mismatch!\n");
        return 1;
    }

    for (size_t i = 0; i < count_naive; i++) {
        if (out_naive[i] != out_simd[i]) {
            fprintf(stderr, "FATAL ERROR: Data mismatch at index %zu (Expected %u, got %u)\n", 
                    i, out_naive[i], out_simd[i]);
            return 1;
        }
    }
    printf("[+] Correctness check: PASSED\n");

    printf("Naive Time: %.2f ms\n", time_naive_ms);
    printf("SIMD Time:  %.2f ms\n", time_simd_ms);
    
    double speedup = time_naive_ms / time_simd_ms;
    printf("Speedup:    %.2fx\n", speedup);

    if (speedup < 3.5) {
        fprintf(stderr, "PERF ERROR: SIMD version is too slow! (Expected at least 2.5x speedup)\n");
        return 1;
    }
    printf("[+] Performance check: PASSED\n");

    free(xs); free(ys); free(zs);
    free(out_naive); free(out_simd);
    return 0;
}