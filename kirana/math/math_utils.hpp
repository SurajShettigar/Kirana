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

} // namespace kirana::math

#endif