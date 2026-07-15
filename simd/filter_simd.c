#include "filter.h"
#include <immintrin.h>

size_t filter_points_simd(
    const float* xs, const float* ys, const float* zs, 
    size_t count, 
    BoundingBox box, 
    uint32_t* out_indices) 
{
    size_t out_count = 0;
    size_t i = 0;

    // границы Bounding Box
    __m256 v_x_min = _mm256_set1_ps(box.x_min);
    __m256 v_x_max = _mm256_set1_ps(box.x_max);
    __m256 v_y_min = _mm256_set1_ps(box.y_min);
    __m256 v_y_max = _mm256_set1_ps(box.y_max);
    __m256 v_z_min = _mm256_set1_ps(box.z_min);
    __m256 v_z_max = _mm256_set1_ps(box.z_max);

    // Обрабатываем по 8 float-ов за раз
    for (; i + 8 <= count; i += 8) {
        __m256 x = _mm256_loadu_ps(&xs[i]);
        __m256 y = _mm256_loadu_ps(&ys[i]);
        __m256 z = _mm256_loadu_ps(&zs[i]);

        __m256 x_ge = _mm256_cmp_ps(x, v_x_min, _CMP_GE_OQ);
        __m256 x_le = _mm256_cmp_ps(x, v_x_max, _CMP_LE_OQ);
        __m256 y_ge = _mm256_cmp_ps(y, v_y_min, _CMP_GE_OQ);
        __m256 y_le = _mm256_cmp_ps(y, v_y_max, _CMP_LE_OQ);
        __m256 z_ge = _mm256_cmp_ps(z, v_z_min, _CMP_GE_OQ);
        __m256 z_le = _mm256_cmp_ps(z, v_z_max, _CMP_LE_OQ);

        __m256 mask_x = _mm256_and_ps(x_ge, x_le);
        __m256 mask_y = _mm256_and_ps(y_ge, y_le);
        __m256 mask_z = _mm256_and_ps(z_ge, z_le);
        
        __m256 mask_xy = _mm256_and_ps(mask_x, mask_y);
        __m256 mask_all = _mm256_and_ps(mask_xy, mask_z);

        // Каждый бит числа соответствует результату сравнения одной из 8 точек
        int bitmask = _mm256_movemask_ps(mask_all);

        while (bitmask) {
            int bit_idx = __builtin_ctz(bitmask);
            
            out_indices[out_count++] = i + bit_idx;
            
            bitmask &= (bitmask - 1);
        }
    }

    // process unaligned tail
    for (; i < count; i++) {
        if (xs[i] >= box.x_min && xs[i] <= box.x_max &&
            ys[i] >= box.y_min && ys[i] <= box.y_max &&
            zs[i] >= box.z_min && zs[i] <= box.z_max) {
            out_indices[out_count++] = i;
        }
    }

    return out_count;
}
