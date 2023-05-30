#ifndef UTIL_HPP
#define UTIL_HPP

#include "common.h"

#define AS_BYTE_PTR(P) ((u8*)P)
#define TO_BYTE_PTR(V) ((u8*)&V)

// Contains implementations for common functions.
// Uses less space than pulling in libc headers.

/*
 * Returns the length of the string in bytes.
 */
u32 strlen(const char* str);

/*
 * Copies string src to string dest.
 */
void strcpy(char* dest, const char* src);

/*
 * Writes the string value of integer value to buffer.
 * Returns the number of characters written.
 */
u32 buffer_int(char* buffer, i32 value, u8 base = 10, u8 flags = 0, u8 minDigits = 0);

/*
 * Writes the hexadecimal string value of integer value to buffer at offset.
 * Returns the new offset after writing the digits.
 */
//u16 buffer_hex(char* buffer, u16 offset, i32 value, u8 num_digits);

/*
 * Writes the binary string value of integer value to buffer at offset.
 * Returns the new offset after writing the digits.
 */
//u16 buffer_bin(char* buffer, u16 offset, i32 value, u8 num_digits);

/*
 * Writes a formatted string to dest using str as the format,
 * and the variadic arguments.
 */
void strfmt(char* dest, const char* str, ...);

/*
 * Copies a specified number of bytes from src to dest.
 */
void memcpy(u8* dest, const u8* src, u32 len);

/*
 * Writes value to len bytes starting at buff.
 */
void memset(u8* buff, u32 len, u8 value);

/*
 * Swaps len bytes between blocks a and b.
 * The two blocks may not overlap.
 */
void memswap(u8* restrict a, u8* restrict b, u32 len);

//Math

/*
 * Returns the absolute value of x.
 */
constexpr i32 abs(i32 x) { return (x < 0)? -x : x; }

/*
 * Returns -1 if x is negative, 0 if x is 0,
 * and 1 if x is positive.
 */
constexpr i32 sgn(i32 x) { return (x == 0)? 0 : ((x < 0)? -1 : 1); }

/*
 * Returns the linear interpolation of t between a and b.
 */
constexpr f32 lerp(f32 a, f32 b, f32 t) 
{
    return a + t * (b - a);
}

/*
 * Returns a raised to the bth power.
 */
constexpr f64 pow(f64 a, i64 b)
{
    if(b > 0)
        return a*pow(a,b-1);
    else if(b < 0)
        return 1.0/(pow(a,-b));
    return 1.0;
}

/*
 * Returns the integer x raised to the nth power.
 */
constexpr i32 ipow(i32 x, i32 n) { 
    if(n == 0) return 1;
    if(n == 1) return x;
    if(n == 2) return (x * x);
    if(n < 0) return 0;
    i32 out = 1;
    while(n > 0)
    {
        out *= x;
        n--;
    }
    return out;
}

/*
 * Returns the square root of x
 */
#define sqrt(x) __builtin_sqrt(x)

/*
 * Count ones
 */
#if 0
constexpr u32 popcount(u32 value)
{
    u32 count = 0;
    for(int i = 0; i < 32; i++)
    {
        if(value & (1 << i)) count++;
    }
    return count;
}
#else 
#define popcount(value) __builtin_popcount(value)
#endif

constexpr i32 clamp(i32 value, i32 min, i32 max)
{
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

constexpr i32 roundi(f32 n)
{
    return (n < 0)? (i32)(n - 0.5f): (i32)(n + 0.5f);
}

#endif //UTIL_HPP