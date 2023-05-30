#pragma once 

#include "common.h"

// An encapsulated 32-bit pointer value to save on storage memory.
// Regular pointers are stored in 64-bits which wastes space and 
// makes structs align to 8-byte boundries.
// With this pointer, we can store pointers and size information 
// in just 4 bytes.
// C++ hates when you do unsafe casting in constexpr context, but
// I still tried.
template<typename T>
union TinyArrayPtr {
    u32 value;
    struct { u16 ptr; u16 len; };

    TinyArrayPtr() = default;
    constexpr TinyArrayPtr(T* p)
        : ptr((u16)((u64)(p) & 0xFFFF)), len(1) {}
    constexpr TinyArrayPtr(T* p, u16 size)
        : ptr((u16)((u64)p & 0xFFFF)), len(size) {}

    constexpr operator T*() { return ((T*)ptr); }
    constexpr operator const T*() const { return ((T*)ptr); }

    constexpr T* operator++() { 
        ptr += sizeof(T);
        return ((T*)(u64)ptr);
    }

    constexpr T* operator++(int)
    {
        T* copy{(T*)(u64)ptr};
        ++*this;         // call the prefix increment
        return copy;
    }

    constexpr T* get() { return ((T*)(u64)ptr); }
    constexpr const T* get() const { return ((T*)(u64)ptr); }

    constexpr T& operator*() { return *((T*)(u64)ptr); }
    constexpr const T& operator*() const { return *((T*)(u64)ptr); }

    constexpr T& operator[](unsigned index) { return ((T*)(u64)ptr)[index]; }
    constexpr const T& operator[](unsigned index) const { return ((T*)(u64)ptr)[index]; }

    constexpr T* operator->() { return ((T*)(u64)ptr); }
    constexpr const T* operator->() const { return ((T*)(u64)ptr); }
};
constexpr u32 tap_size = sizeof(TinyArrayPtr<u8>);

// An encapsulated 16-bit pointer value to save on storage memory.
// Regular pointers are stored in 64-bits which wastes space and 
// makes structs align to 8-byte boundries.
// With this pointer, we can store pointers in just two bytes.
// C++ hates when you do unsafe casting in constexpr context, but
// I still tried.
template<typename T>
struct TinyPtr {
    u16 ptr;

    TinyPtr() = default;
    constexpr TinyPtr(T* p)
        : ptr((u16)((u64)(p) & 0xFFFF)) {}

    constexpr operator T*() { return ((T*)(u64)ptr); }
    constexpr operator const T*() const { return ((T*)(u64)ptr); }

    constexpr T* operator++() { 
        ptr += sizeof(T);
        return ((T*)(u64)ptr);
    }

    constexpr T* operator++(int)
    {
        T* copy{(T*)(u64)ptr};
        ++*this;         // call the prefix increment
        return copy;
    }

    constexpr T* get() { return ((T*)(u64)ptr); }
    constexpr const T* get() const { return ((T*)(u64)ptr); }

    constexpr T& operator*() { return *((T*)(u64)ptr); }
    constexpr const T& operator*() const { return *((T*)(u64)ptr); }

    constexpr T* operator->() { return ((T*)(u64)ptr); }
    constexpr const T* operator->() const { return ((T*)(u64)ptr); }

    // Used for TinyPtr<Ret(Args...)> function pointers.
    template<typename ...Args>
    constexpr auto operator()(Args... args)
    {
        return ((T*)((u64)ptr))(args...);
    }
};
constexpr u32 tp_size = sizeof(TinyPtr<u8>);

#define PTR(T, X) ((T*)(u64)(X))
#define DEREF(T, X) *((T*)(u64)(X))
#define PACK_PTR(X) ((u16)(u64)(X))
#define PACK_ARR_PTR(X, N) (((u32)(X) << 16) | ((u32)(N) & 0xFFFF))
