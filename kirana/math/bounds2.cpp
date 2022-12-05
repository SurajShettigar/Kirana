#include "bounds2.hpp"
#include "ray.hpp"
#include "math_utils.hpp"

#include <string>
#include <limits>
#include <cmath>

kirana::math::Bounds2::Bounds2()
    : m_min{Vector2::ONE * std::numeric_limits<float>::max()},
      m_max{Vector2::ONE * std::numeric_limits<float>::lowest()} {};

kirana::math::Bounds2::Bounds2(const Vector2 &point)
    : m_min{point}, m_max{point} {};

kirana::math::Bounds2::Bounds2(const Vector2 &min, const Vector2 &max)
    : m_min{Vector2{std::fmin(min[0], max[0]), std::fmin(min[1], max[1])}},
      m_max{Vector2{std::fmax(min[0], max[0]), std::fmax(min[1], max[1])}} {};


kirana::math::Bounds2::Bounds2(const Bounds2 &bounds)
{
    if (this != &bounds)
    {
        m_min = bounds.m_min;
        m_max = bounds.m_max;
    }
}

kirana::math::Bounds2 &kirana::math::Bounds2::operator=(const Bounds2 &bounds)
{
    if (this != &bounds)
    {
        m_min = bounds.m_min;
        m_max = bounds.m_max;
    }
    return *this;
}

kirana::math::Bounds2::operator std::string() const
{
    return std::string("<Min: ") + std::string(m_min) +
           ", Max: " + std::string(m_max) + ">";
}

void kirana::math::Bounds2::encapsulate(const Vector2 &point)
{
    m_min =
        Vector2(std::fmin(m_min[0], point[0]), std::fmin(m_min[1], point[1]));
    m_max =
        Vector2(std::fmax(m_max[0], point[0]), std::fmax(m_max[1], point[1]));
}

void kirana::math::Bounds2::encapsulate(const Bounds2 &bounds)
{
    m_min = Vector2(std::fmin(m_min[0], bounds.m_min[0]),
                    std::fmin(m_min[1], bounds.m_min[1]));
    m_max = Vector2(std::fmax(m_max[0], bounds.m_max[0]),
                    std::fmax(m_max[1], bounds.m_max[1]));
}

void kirana::math::Bounds2::expand(float delta)
{
    m_min -= Vector2::ONE * delta;
    m_max += Vector2::ONE * delta;
}

bool kirana::math::Bounds2::contains(const Vector2 &point) const
{
    const bool x = (point[0] >= m_min[0]) && (point[0] <= m_max[0]);
    const bool y = (point[1] >= m_min[1]) && (point[1] <= m_max[1]);
    return x && y;
}

kirana::math::Vector2 kirana::math::Bounds2::lerp(const Vector2 &t) const
{
    return Vector2(math::lerp(m_min[0], m_max[0], t[0]),
                   math::lerp(m_min[1], m_max[1], t[1]));
}

bool kirana::math::Bounds2::overlaps(const Bounds2 &lhs, const Bounds2 &rhs)
{
    const bool x =
        (lhs.m_max[0] >= rhs.m_min[0]) && (lhs.m_min[0] <= rhs.m_max[0]);
    const bool y =
        (lhs.m_max[1] >= rhs.m_min[1]) && (lhs.m_min[1] <= rhs.m_max[1]);
    return x && y;
}

kirana::math::Bounds2 kirana::math::Bounds2::intersect(const Bounds2 &lhs,
                                                       const Bounds2 &rhs)
{
    return Bounds2(Vector2(std::fmax(lhs.m_min[0], rhs.m_min[0]),
                           std::fmax(lhs.m_min[1], rhs.m_min[1])),
                   Vector2(std::fmin(lhs.m_max[0], rhs.m_max[0]),
                           std::fmin(lhs.m_max[1], rhs.m_max[1])));
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Bounds2 &bounds)
{
    return out << "<Min: " << bounds.m_min << ", Max: " << bounds.m_max << ">";
}