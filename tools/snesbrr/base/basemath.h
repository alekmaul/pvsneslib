/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef libbase_basemath_h
#define libbase_basemath_h

#include "basetypes.h"

namespace base
{

    namespace math
    {

        template <uint N, typename T>
        inline static bool bit(const T &x)
        {
            return x & (1 << N);
        }

        template <uint N, typename T>
        inline static void bit(T &x, bool b)
        {
            x = (x & ~(1 << N)) | (b << N);
        }

        template <typename T>
        inline static bool bit(uint N, const T &x)
        {
            return x & (1 << N);
        }

        template <typename T>
        inline static void bit(uint N, T &x, bool b)
        {
            x = (x & ~(1 << N)) | (b << N);
        }

        template <uint N, typename T>
        inline static void bit_or(T &x, bool b)
        {
            x |= b << N;
        }

        template <typename T>
        inline static void bit_or(uint N, T &x, bool b)
        {
            x |= b << N;
        }

        template <uint N, typename T>
        inline static void bit_xor(T &x, bool b)
        {
            x ^= b << N;
        }

        template <typename T>
        inline static void bit_xor(uint N, T &x, bool b)
        {
            x ^= b << N;
        }

        template <uint N, typename T>
        inline static T clamp(T x)
        {
            const T low  = static_cast<T>(-1) << (N - 1);
            const T high = (1 << (N - 1)) - 1;

            if (x > high)
                x = high;
            else if (x < low)
                x = low;

            return x;
        }

        template <uint N, typename T>
        inline static T uclamp(T x)
        {
            const T high = (static_cast<T>(1) << N) - 1;

            if (x > high)
                x = high;

            return x;
        }

    } // math

    using namespace math;

} // base

#endif
