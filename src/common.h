#ifndef COMMON_H
#define COMMON_H

#include "wasm4.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;

#define PALETTE_COLOR_0 0x000000
#if 0
#define PALETTE_COLOR_1 0x110c22
#define PALETTE_COLOR_2 0x608fcf
#define PALETTE_COLOR_3 0xc3dce5
#else 
#define PALETTE_COLOR_1 0x808080
#define PALETTE_COLOR_2 0xc0c0c0
#define PALETTE_COLOR_3 0xffffff
#endif

#define PACKED   __attribute__ ((__packed__))
#define NOINLINE __attribute__ ((noinline))
#define COLD     __attribute__ ((cold))

#ifdef __cplusplus
#define restrict __restrict

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

#define UNUSED [[maybe_unused]]

#define assume __builtin_assume
#define expect __builtin_expect

#define drop(x) ((void)x)

#define _s_imp(x) #x
#define _s(x) _s_imp(x)

#ifdef DEBUG
#define assert(x, msg) if(!(x)) { trace("Assert fail at " __FILE__ ", line " _s(__LINE__) ": \"" #x "\". " msg); __builtin_trap(); }
#else 
#define assert(x, msg) ((void)0)
#endif

#if 1
#define SOUND_TEST 0
#else 
#define SOUND_TEST 1
#endif

#define fallthrough [[fallthrough]]

#endif //__cplusplus

#endif //COMMON_H