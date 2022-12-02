#include "bounds2.hpp"
#include "ray.hpp"
#include "math_utils.hpp"

#include <limits>
#include <cmath>

kirana::math::Bounds2::Bounds2()
    : m_min{Vector2::ONE * std::numeric_limits<float>::max()},
      m_max{Vector2::ONE * std::numeric_limits<float>::lowest()},
      m_center{(m_max + m_min) * 0.5f}, m_size{m_max - m_min}, m_extent{
                                                                   m_size *
                                                                   0.5f} {};

kirana::math::Bounds2::Bounds2(const Vector2 &point)
    : m_min{point}, m_max{point}, m_center{point}, m_size{Vector3::ZERO},
      m_extent{Vector3::ZERO} {};

kirana::math::Bounds2::Bounds2(const Vector2 &min, const Vector2 &max)
    : m_min{Vector2{std::fmin(min[0], max[0]), std::fmin(min[1], max[1])}},
      m_max{Vector2{std::fmax(min[0], max[0]), std::fmax(min[1], max[1])}},
      m_center{(m_min + m_max) * 0.5f}, m_size{m_max - m_min}, m_extent{
                                                                   m_size *
                                                                   0.5f} {};


kirana::math::Bounds2::Bounds2(const Bounds2 &bounds)
{
    if (this != &bounds)
    {
        m_min = bounds.m_min;
        m_max = bounds.m_max;
        m_center = bounds.m_center;
        m_size = bounds.m_size;
        m_extent = bounds.m_extent;
    }
}

kirana::math::Bounds2 &kirana::math::Bounds2::operator=(const Bounds2 &bounds)
{
    if (this != &bounds)
    {
        m_min = bounds.m_min;
        m_max = bounds.m_max;
        m_center = bounds.m_center;
        m_size = bounds.m_size;
        m_extent = bounds.m_extent;
    }
    return *this;
}

void kirana::math::Bounds2::encapsulate(const Vector2 &point)
{
    m_min =
        Vector2(std::fmin(m_min[0], point[0]), std::fmin(m_min[1], point[1]));
    m_max =
        Vector2(std::fmax(m_max[0], point[0]), std::fmax(m_max[1], point[1]));
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

void kirana::math::Bounds2::encapsulate(const Bounds2 &bounds)
{
    m_min = Vector2(std::fmin(m_min[0], bounds.m_min[0]),
                    std::fmin(m_min[1], bounds.m_min[1]));
    m_max = Vector2(std::fmax(m_max[0], bounds.m_max[0]),
                    std::fmax(m_max[1], bounds.m_max[1]));
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

void kirana::math::Bounds2::expand(float delta)
{
    m_min -= Vector2::ONE * delta;
    m_max += Vector2::ONE * delta;
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

bool kirana::math::Bounds2::contains(const Vector2 &point)
{
    const bool x = (point[0] >= m_min[0]) && (point[0] <= m_max[0]);
    const bool y = (point[1] >= m_min[1]) && (point[1] <= m_max[1]);
    return x && y;
}

kirana::math::Vector2 kirana::math::Bounds2::lerp(const Vector2 &t)
{
    return Vector2(math::lerp(m_min[0], m_max[0], t[0]),
                   math::lerp(m_min[1], m_max[1], t[1]));
}

kirana::math::Bounds2 kirana::math::Bounds2::createFromCenterSize(
    const Vector2 &center, const Vector2 &size)
{
    Bounds2 bounds;
    bounds.m_center = center;
    bounds.m_size = size;
    bounds.m_extent = bounds.m_size * 0.5;
    bounds.m_max = bounds.m_center + bounds.m_extent;
    bounds.m_min = bounds.m_center - bounds.m_extent;

    return bounds;
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
    return out << "<Min: " << bounds.m_min << ", Max: " << bounds.m_max
               << ", Center: " << bounds.m_center << ", Size: " << bounds.m_size
               << ", Extent: " << bounds.m_extent << ">";
}