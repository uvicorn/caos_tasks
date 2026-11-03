#ifndef RANDOM_H
#define RANDOM_H

#include "stdint.h"
#include "stddef.h"


void xorshift32_setSeed(uint32_t seed);
uint32_t xorshift32();

void xorshift_array(uint32_t* output_array, size_t size, uint32_t seed);
void xorshift_array_bounded(uint32_t* output_array, size_t size, uint32_t seed, uint32_t bound);

#endif