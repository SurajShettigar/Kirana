#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <limits>

namespace kirana::math
{

static constexpr double PI = 3.141592653589793;
static constexpr float DEGREE_360 = 360.0f;
static constexpr float DEGREE_180 = 180.0f;
static constexpr float DEGREE_90 = 90.0f;
static constexpr float DEGREE_45 = 45.0f;
static constexpr float MACHINE_EPSILON_F =
    std::numeric_limits<float>::epsilon() * 0.5f;
static constexpr double MACHINE_EPSILON_D =
    std::numeric_limits<double>::epsilon() * 0.5;

/// Computed floating-point error. (Refer Section 3.9.1 in PBRT 3rd Edition)
inline constexpr double gamma(int n)
{
    return (static_cast<double>(n) * MACHINE_EPSILON_D) /
           (1.0 - static_cast<double>(n) * MACHINE_EPSILON_D);
}

/// Computed floating-point error. (Refer Section 3.9.1 in PBRT 3rd Edition)
inline constexpr float gammaf(int n)
{
    return (static_cast<float>(n) * MACHINE_EPSILON_F) /
           (1.0f - static_cast<float>(n) * MACHINE_EPSILON_F);
}

inline float radians(float degrees)
{
    return (static_cast<float>(PI) / DEGREE_180) * degrees;
}

inline float degrees(float radians)
{
    return (DEGREE_180 / static_cast<float>(PI)) * radians;
}

inline bool approximatelyEqual(float x, float y)
{
    return fabsf(x - y) <= std::numeric_limits<float>::epsilon();
}

inline float lerp(float x, float y, float t)
{
    return (1.0f - t) * x + t * y;
}

inline double clamp(double x, double min, double max)
{
    return std::fmax(min, std::fmin(x, max));
}

inline float clampf(float x, float min, float max)
{
    return std::fmaxf(min, std::fminf(x, max));
}

template <typename T> inline T map(T x, T inMin, T inMax, T outMin, T outMax)
{
    double val =
        static_cast<double>(outMin) + static_cast<double>(x - inMin) *
                                          static_cast<double>(outMax - outMin) /
                                          static_cast<double>(inMax - inMin);
    return static_cast<T>(clamp(val, outMin, outMax));
}

/**
 * Clamps the Euler angles to it's canonical range.
 * @param x Clamps x to [-90.0, 90.0]
 * @param y Clamps y to [-180.0, 180.0]
 * @param z Clamps z to [-180.0, 180.0], but if x = +-90.0, z will be 0
 */
inline void clampEulerAngles(float *x, float *y, float *z)
{
    *x = std::fmaxf(-DEGREE_90, std::fminf(*x, DEGREE_90));
    *y = std::fmaxf(-DEGREE_180, std::fminf(*y, DEGREE_180));
    *z = std::fmaxf(-DEGREE_180, std::fminf(*z, DEGREE_180));
    if (approximatelyEqual(*x, DEGREE_90) || approximatelyEqual(*x, -DEGREE_90))
        *z = 0.0f;
}

/// Wraps an angle to range [-180.0, 180.0]
inline float wrapAngle(float angle)
{
    return angle -
           (DEGREE_360 * std::floorf((angle + DEGREE_180) / DEGREE_360));
}

} // namespace kirana::math

#endif