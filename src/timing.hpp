#pragma once

#include "common.h"

// The number of frames in 1 second.
#define FRAMES_PER_SEC 60

// A macro which calculates the number of frames in s seconds.
#define SEC(s) ((s)*FRAMES_PER_SEC)

// A macro which calculates the number of frames in m minutes.
#define MIN(m) ((m)*60*FRAMES_PER_SEC)

// A convience macro which calculates the number of frames
// in m minutes and s seconds.
#define MIN_SEC(m, s) (MIN(m) + SEC(s))

