#include "util.hpp"
#include <stdarg.h>

#define STRFMT_HEX 0
#define STRFMT_BIN 0

u32 strlen(const char* str)
{
    u32 len = 0;
    while(str[len] != '\0')
        len++;
    return len;
}

void strcpy(char* dest, const char* src)
{   
    u32 len = 0;
    for(; src[len] != '\0'; len++)
        dest[len] = src[len];
    dest[len] = '\0';
}

static i32 power_of_ten[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
};

#if STRFMT_HEX
static const char hexstr[] = "0123456789ABCDEF";
#endif

#define FFLAG_ZERO_PADDED (1     )
#define FFLAG_0X_PREFIX   (1 << 1)

u32 buffer_int(char* buffer, i32 value, u8 base, u8 flags, u8 minDigits)
{
    u8 num_digits = (base == 16)? 7 : (base == 10)? 7: 16;
    u32 count = 0;
    if(num_digits == 0) 
        return 0;
    if(base != 2 && base != 10 && base != 16)
        return 0;
    if(base == 10)
    {
        // Add negative sign if negative.
        if(value < 0)
        {
            *(buffer++) = '-';
            count++;
            value = value * -1;
        }
    }
#if STRFMT_HEX
    if(base == 16 && (flags & FFLAG_0X_PREFIX))
    {
        *(buffer++) = '0';
        count++;
        *(buffer++) = 'x';
        count++;
    }
#endif
#if STRFMT_BIN
    if(base == 2 && (flags & FFLAG_0X_PREFIX))
    {
        *(buffer++) = '0';
        count++;
        *(buffer++) = 'b';
        count++;
    }
#endif 
#if !STRFMT_BIN && !STRFMT_HEX
    while(num_digits > 0 && value / power_of_ten[num_digits] == 0)
    {
        if(num_digits < minDigits)
        {
            *(buffer++) = (flags & FFLAG_ZERO_PADDED)? '0' : ' ';
            count++;
        }
        num_digits--;
    }
#else
    switch(base) 
    {
    case 10:
        while(num_digits > 0 && value / power_of_ten[num_digits] == 0)
        {
            if(num_digits < minDigits)
            {
                *(buffer++) = (flags & FFLAG_ZERO_PADDED)? '0' : ' ';
                count++;
            }
            num_digits--;
        }
        break;
    case 2:
        while(num_digits > 0 && value / (1 << num_digits) == 0)
        {
            if(num_digits < minDigits)
            {
                *(buffer++) = (flags & FFLAG_ZERO_PADDED)? '0' : ' ';
                count++;
            }
            num_digits--;
        }
        break;
    case 16:
        while(num_digits > 0 && ((value & (0xF << (num_digits * 4))) == 0))
        {
            if(num_digits < minDigits)
            {
                *(buffer++) = (flags & FFLAG_ZERO_PADDED)? '0' : ' ';
                count++;
            }
            num_digits--;
        }
        break;
    }
#endif
    u32 i = num_digits;
#if !STRFMT_BIN && !STRFMT_HEX
    if(num_digits == 0)
    {
        *buffer = '0' + (char)(value % 10);
    }
    while(i > 0)
    {
        buffer[(num_digits - i)] = '0' + (char)((value / power_of_ten[i]) % 10);
        i--;
    }
    buffer[num_digits] = '0' + (char)(value % 10);
#else
    if(num_digits == 0)
    {
        switch(base)
        {
        case 10:
            *buffer = '0' + (char)(value % 10);
            break;
        case 2:
            *buffer = '0' + (char)(value & 1);
            break;
        case 16:
            *buffer = hexstr[value & 0xF];
            break;
        }
        return count + 1;
    }
    while(i > 0)
    {
        switch(base)
        {
        case 10:
            buffer[(num_digits - i)] = '0' + (char)((value / power_of_ten[i]) % 10);
            break;
        case 2:
            buffer[(num_digits - i)] = '0' + (char)((value / (1 << i)) % 2);
            break;
        case 16:
            buffer[(num_digits - i)] = hexstr[(value & (0xF << (4*i))) >> (4*i)];
            break;
        }
        
        i--;
    }
    switch(base)
    {
    case 10:
        buffer[num_digits] = '0' + (char)(value % 10);
        break;
    case 2:
        buffer[num_digits] = '0' + (char)(value & 1);
        break;
    case 16:
        buffer[num_digits] = hexstr[value & 0xF];
        break;
    }
#endif
    return count + num_digits + 1;
}

#define IS_DIGIT(X) ((X) >= '0' && (X) <= '9')

void strfmt(char* dest, const char* str, ...)
{
    va_list list;
    va_start(list, str);
    u32 idx = 0;
    u32 idx2 = 0;
    while(str[idx] != '\0')
    {
        // Reset format specifier values
        u8 flags = 0;
        u8 digitCount = 0;
        
        // Copy string until hitting a % or NUL char.
        while(str[idx] != '%')
        {
            dest[idx2] = str[idx];
            // If we find a NUL, terminate.
            if(str[idx] == '\0')
            {
                va_end(list);
                return;
            }
            idx++, idx2++;
        }
        idx++;

        // Check for another %
        if(str[idx] == '%')
        {
            dest[idx2] = '%';
            idx++, idx2++;
            continue;
        }

        // Check for flags
        while(1)
        {
            if(str[idx] == '\0')
            {
                dest[idx2] = str[idx];
                va_end(list);
                return;
            }
            else if(str[idx] == '0')
            {
                flags |= FFLAG_ZERO_PADDED;
                idx++;
            }
            else if(str[idx] == '#')
            {
                flags |= FFLAG_0X_PREFIX;
                idx++;
            }
            else break;
        }

        // Check for width
        if(IS_DIGIT(str[idx]))
        {
            digitCount = (u8)(str[idx] - '0');
            idx ++;
        }

        // Check specifier
        if(str[idx] == 's')
        {
            auto s = va_arg(list, const char*);
            for(u32 i = 0; s[i] != '\0'; i++)
            {
                dest[idx2++] = s[i];
            }
            idx++;
            continue;
        }
        else if(str[idx] == 'd')
        {
            auto d = va_arg(list, i32);
            idx2 += buffer_int(&dest[idx2], d, 10, flags, digitCount);
            idx++;
            continue;
        }
#if STRFMT_HEX
        else if(str[idx] == 'X')
        {
            auto d = va_arg(list, i32);
            idx2 += buffer_int(&dest[idx2], d, 16, flags, digitCount);
            idx++;
            continue;
        }
#endif
#if STRFMT_BIN
        else if(str[idx] == 'b')
        {
            auto d = va_arg(list, i32);
            idx2 += buffer_int(&dest[idx2], d, 2, flags, digitCount);
            idx++;
            continue;
        }
#endif
        else if(str[idx] == 'c')
        {
            auto d = (char)va_arg(list, int);
            if(d != 0)
                dest[idx2++] = d;
            idx++;
            continue;
        }
    }
    dest[idx2] = '\0';
    va_end(list);
}

void memcpy(u8* dest, const u8* src, u32 len)
{
    for(u32 i = 0; i < len; i++) 
        dest[i] = src[i];
}

void memset(u8* buff, u32 len, u8 value)
{
    for(u32 i = 0; i < len; i++) 
        buff[i] = value;
}

void memswap(u8* restrict a, u8* restrict b, u32 len)
{
    for(u32 i = 0; i < len; i++)
    {
        u8 temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}
