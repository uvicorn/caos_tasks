#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

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

uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

float rand_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

static int run_case(
    const char* name,
    const float* xs,
    const float* ys,
    const float* zs,
    size_t count,
    BoundingBox box,
    const uint32_t* expected_indices,
    size_t expected_count)
{
    size_t out_size = count > 0 ? count : 1;
    uint32_t* out_naive = malloc(out_size * sizeof(uint32_t));
    uint32_t* out_simd = malloc(out_size * sizeof(uint32_t));
    assert(out_naive != NULL && out_simd != NULL);

    size_t count_naive = filter_points_naive(xs, ys, zs, count, box, out_naive);
    size_t count_simd = filter_points_simd(xs, ys, zs, count, box, out_simd);

    if (count_naive != expected_count || count_simd != expected_count) {
        fprintf(stderr, "[!] Edge case failed: %s (expected %zu matches, got %zu/%zu)\n",
                name, expected_count, count_naive, count_simd);
        free(out_naive);
        free(out_simd);
        return 0;
    }

    for (size_t i = 0; i < expected_count; i++) {
        if (out_naive[i] != expected_indices[i] || out_simd[i] != expected_indices[i]) {
            fprintf(stderr, "[!] Edge case failed: %s at index %zu (expected %u)\n",
                    name, i, expected_indices[i]);
            free(out_naive);
            free(out_simd);
            return 0;
        }
    }

    printf("  [ok] %s\n", name);
    free(out_naive);
    free(out_simd);
    return 1;
}

static int run_edge_case_tests(void) {
    printf("[+] Running edge-case tests...\n");

    const float xs_empty[] = {0.0f};
    const float ys_empty[] = {0.0f};
    const float zs_empty[] = {0.0f};
    const uint32_t expected_empty[] = {0};
    BoundingBox empty_box = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    if (!run_case("empty input", xs_empty, ys_empty, zs_empty, 0, empty_box, expected_empty, 0)) {
        return 0;
    }

    const float xs_boundary[] = {-10.0f, 5.0f, 10.0f, 100.0f};
    const float ys_boundary[] = {0.0f, 20.0f, 50.0f, 50.0f};
    const float zs_boundary[] = {-2.0f, 1.0f, 3.0f, 0.0f};
    const uint32_t expected_boundary[] = {0, 1, 2};
    BoundingBox boundary_box = {-10.0f, 10.0f, 0.0f, 50.0f, -2.0f, 3.0f};
    if (!run_case("boundary values", xs_boundary, ys_boundary, zs_boundary, 4, boundary_box,
                  expected_boundary, 3)) {
        return 0;
    }

    const float xs_outside[] = {100.0f, -100.0f, 20.0f, -50.0f};
    const float ys_outside[] = {100.0f, -100.0f, 20.0f, 60.0f};
    const float zs_outside[] = {10.0f, -10.0f, 4.0f, -3.0f};
    const uint32_t expected_outside[] = {0};
    if (!run_case("all points outside", xs_outside, ys_outside, zs_outside, 4, boundary_box,
                  expected_outside, 0)) {
        return 0;
    }

    const float xs_tail[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    const float ys_tail[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const float zs_tail[] = {-1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f};
    uint32_t expected_tail[9];
    for (size_t i = 0; i < 9; i++) {
        expected_tail[i] = (uint32_t)i;
    }
    if (!run_case("tail handling (9 elements)", xs_tail, ys_tail, zs_tail, 9, boundary_box,
                  expected_tail, 9)) {
        return 0;
    }

    const float xs_exact_vector[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    const float ys_exact_vector[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    const float zs_exact_vector[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    uint32_t expected_exact_vector[8];
    for (size_t i = 0; i < 8; i++) {
        expected_exact_vector[i] = (uint32_t)i;
    }
    if (!run_case("vector-width boundary (8 elements)", xs_exact_vector, ys_exact_vector,
                  zs_exact_vector, 8, boundary_box, expected_exact_vector, 8)) {
        return 0;
    }

    printf("[+] All edge-case tests passed\n");
    return 1;
}

static void generate_points(float* xs, float* ys, float* zs, size_t count) {
    for (size_t i = 0; i < count; i++) {
        xs[i] = rand_float(-100.0f, 100.0f);
        ys[i] = rand_float(-100.0f, 100.0f);
        zs[i] = rand_float(-10.0f, 10.0f);
    }
}

static int run_benchmark_iteration(
    const float* xs,
    const float* ys,
    const float* zs,
    size_t count,
    BoundingBox box,
    uint32_t* out_naive,
    uint32_t* out_simd,
    double* time_naive_ms,
    double* time_simd_ms,
    size_t* count_naive,
    size_t* count_simd)
{
    uint64_t t1 = get_time_ns();
    *count_naive = filter_points_naive(xs, ys, zs, count, box, out_naive);
    uint64_t t2 = get_time_ns();
    *time_naive_ms = (t2 - t1) / 1000000.0;

    uint64_t t3 = get_time_ns();
    *count_simd = filter_points_simd(xs, ys, zs, count, box, out_simd);
    uint64_t t4 = get_time_ns();
    *time_simd_ms = (t4 - t3) / 1000000.0;

    if (*count_naive != *count_simd) {
        fprintf(stderr, "FATAL ERROR: Output counts mismatch!\n");
        return 0;
    }

    for (size_t i = 0; i < *count_naive; i++) {
        if (out_naive[i] != out_simd[i]) {
            fprintf(stderr, "FATAL ERROR: Data mismatch at index %zu (Expected %u, got %u)\n",
                    i, out_naive[i], out_simd[i]);
            return 0;
        }
    }

    return 1;
}

int main(void) {
    printf("[*] Starting SIMD LiDAR Filter Test...\n");

    if (!run_edge_case_tests()) {
        return 1;
    }

    const size_t COUNT = 30 * 1024 * 1024;
    const int ITERATIONS = 16;

    float* xs = aligned_alloc(32, COUNT * sizeof(float));
    float* ys = aligned_alloc(32, COUNT * sizeof(float));
    float* zs = aligned_alloc(32, COUNT * sizeof(float));
    uint32_t* out_naive = aligned_alloc(32, COUNT * sizeof(uint32_t));
    uint32_t* out_simd = aligned_alloc(32, COUNT * sizeof(uint32_t));

    assert(xs != NULL && ys != NULL && zs != NULL && out_naive != NULL && out_simd != NULL);

    printf("[+] Allocating memory and running %d benchmark iterations...\n", ITERATIONS);
    srand((unsigned int)time(NULL));

    BoundingBox box = {
        .x_min = -10.0f, .x_max = 10.0f,
        .y_min = 0.0f,   .y_max = 50.0f,
        .z_min = -2.0f,  .z_max = 3.0f
    };

    double total_time_naive_ms = 0.0;
    double total_time_simd_ms = 0.0;
    size_t total_count_naive = 0;
    size_t total_count_simd = 0;

    for (int iter = 0; iter < ITERATIONS; iter++) {
        generate_points(xs, ys, zs, COUNT);
        double time_naive_ms = 0.0;
        double time_simd_ms = 0.0;
        size_t count_naive = 0;
        size_t count_simd = 0;

        if (!run_benchmark_iteration(xs, ys, zs, COUNT, box, out_naive, out_simd,
                                     &time_naive_ms, &time_simd_ms,
                                     &count_naive, &count_simd)) {
            free(xs);
            free(ys);
            free(zs);
            free(out_naive);
            free(out_simd);
            return 1;
        }

        total_time_naive_ms += time_naive_ms;
        total_time_simd_ms += time_simd_ms;
        total_count_naive += count_naive;
        total_count_simd += count_simd;
    }

    double avg_time_naive_ms = total_time_naive_ms / ITERATIONS;
    double avg_time_simd_ms = total_time_simd_ms / ITERATIONS;
    size_t avg_count_naive = total_count_naive / ITERATIONS;
    size_t avg_count_simd = total_count_simd / ITERATIONS;

    printf("\n--- AVERAGED RESULTS (16 runs) ---\n");
    printf("Points found: %zu (Naive), %zu (SIMD)\n", avg_count_naive, avg_count_simd);
    printf("[+] Correctness check: PASSED\n");
    printf("Naive Time (avg): %.2f ms\n", avg_time_naive_ms);
    printf("SIMD Time (avg):  %.2f ms\n", avg_time_simd_ms);

    double speedup = avg_time_naive_ms / avg_time_simd_ms;
    printf("Speedup:    %.2fx\n", speedup);

    if (speedup < 3.5) {
        fprintf(stderr, "PERF ERROR: SIMD version is too slow! (Expected at least 3.5x speedup)\n");
        free(xs);
        free(ys);
        free(zs);
        free(out_naive);
        free(out_simd);
        return 1;
    }
    printf("[+] Performance check: PASSED\n");

    free(xs);
    free(ys);
    free(zs);
    free(out_naive);
    free(out_simd);
    return 0;
}