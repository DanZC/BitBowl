#pragma once
#include "wasm4.h"

#define NETPLAY_ACTIVE (*NETPLAY & 0b100)
#define NETPLAY_PIDX   (*NETPLAY & 0b011)

