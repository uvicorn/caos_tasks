#include "stddef.h"
#include "random.h"
#include "immintrin.h"
#include "assert.h"
#include <stdint.h>
// #include "nmmintrin.h"


uint32_t __XORSHIFT_SEED = 123421;

/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
uint32_t xorshift32()
{
    uint32_t x = __XORSHIFT_SEED;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return __XORSHIFT_SEED = x;
}

void xorshift32_setSeed(uint32_t seed){
    __XORSHIFT_SEED = seed;
}

void rand_string(unsigned char* buf, size_t len){
    int i = 0;

    while(i < len){
        uint32_t rand = xorshift32();

        for (size_t j = 0; j < 4 && i < len; j++){
            *buf++ = rand & 0xff;
            rand >>= 8;
            i++;
        }
    }
}


/**
 * Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
 * Writen with AVX2
 */
void xorshift_array(uint32_t* output_array, size_t size, uint32_t seed){

#ifndef __AVX2__

    uint32_t x = seed;
    while (size--){
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *output_array++ = x;
    }

#else

    // БРАТУХА не смог в AVX2
    // FIXME
    uint32_t x = seed;
    while (size--){
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *output_array++ = x;
    }


#endif
}


/**
 * Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
 * Writen with AVX2. Result numbers will & (bound - 1)
 */
void xorshift_array_bounded(uint32_t* output_array, size_t size, uint32_t seed, uint32_t bound){

#ifndef __AVX2__

    uint32_t x = seed;
    while (size--){
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *output_array++ = x % bound;
    }

#else

    // БРАТУХА не смог в AVX2
    // FIXME

    uint32_t x = seed;
    while (size--){
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *output_array++ = x % bound;
    }

#endif
}