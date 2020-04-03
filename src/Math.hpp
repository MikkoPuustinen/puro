#pragma once

struct Math
{

    template <typename FloatType>
    static FloatType pi()
    {
        return static_cast<FloatType> (3.14159265358979323846);
    }

    template <typename FloatType>
    static void multiplyAdd(FloatType* dst, FloatType* src1, FloatType* src2, const int n)
    {
        for (int i = 0; i < n; ++i)
            dst[i] += src1[i] * src2[i];
    };

    /** In-place sin */
    template <typename FloatType>
    static void sin(FloatType* buf, const int n)
    {
        for (int i=0; i<n; ++i)
            buf[i] = std::sin(buf[i]);
    }

    /** In-place cos */
    template <typename FloatType>
    static void cos(FloatType* buf, const int n)
    {
        for (int i=0; i<n; ++i)
            buf[i] = std::cos(buf[i]);
    }
};
