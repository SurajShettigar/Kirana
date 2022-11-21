#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <limits>

namespace kirana::math
{

static constexpr double PI = 3.141592653589793;

inline float radians(float degrees)
{
    return (static_cast<float>(PI) / 180.0f) * degrees;
}

inline float degrees(float radians)
{
    return (180.0f / static_cast<float>(PI)) * radians;
}

inline bool approximatelyEqual(float x, float y)
{
    return fabsf(x - y) <= std::numeric_limits<float>::epsilon();
}

inline bool approximatelyEqual(double x, double y)
{
    return abs(x - y) <= std::numeric_limits<double>::epsilon();
}

/**
 * Clamps the Euler angles to it's canonical range.
 * @param x Clamps x to [-90.0, 90.0]
 * @param y Clamps y to [-180.0, 180.0]
 * @param z Clamps z to [-180.0, 180.0], but if x = +-90.0, z will be 0
 */
inline void clampEulerAngles(float *x, float *y, float *z)
{
    std::fmaxf(-90.0f, std::fminf(*x, 90.0f));
    std::fmaxf(-180.0f, std::fminf(*y, 180.0f));
    std::fmaxf(-180.0f, std::fminf(*z, 180.0f));
    if (approximatelyEqual(*x, 90.0f) || approximatelyEqual(*x, -90.0f))
        *z = 0.0f;
}

/// Wraps an angle to range [-180.0, 180.0]
inline float wrapAngle(float angle)
{
    return angle - (360.0f * std::floorf((angle + 180.0f) / 360.0f));
}

} // namespace kirana::math

#endif