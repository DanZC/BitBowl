#include "random.hpp"

f32 random_float()
{
    f32 f = 0.f;
    u32 u = pcg32_random();
    //Create the float using a sequence of fractional additions.
    for(int i = 0; i < 31; i++)
    {
        f += (u&1) * (1.f/((float)(1 << (i+1))));
        u >>= 1;
    }
    return f;
    //f32 f = 0.f;
    //u32 u = *((u32*)&f);
    //u32 exp_mask = (1u << 23) - 1;
    //u |= pcg32_random_r(&sRandState) & exp_mask;
    //return *((f32*)&u);
}
