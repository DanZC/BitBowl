#pragma once

extern "C" {
#include "pcg/pcg_basic.h"
}

#include "common.h"

// Using minimal pcg implementation. 

#define random_int() (pcg32_random())
#define random_int_bound(max) (pcg32_boundedrand(max+1))
#define random_int_range(min, max) (pcg32_boundedrand((max+1)-min) + min)

f32 random_float();

#define random_seed(value) pcg32_srandom(value, (uintptr_t)&random_float)
