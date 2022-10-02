#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <limits>

namespace kirana::math
{

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