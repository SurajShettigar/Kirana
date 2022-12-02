#include "bounds3.hpp"
#include "ray.hpp"
#include "math_utils.hpp"

#include <limits>
#include <cmath>

kirana::math::Bounds3::Bounds3()
    : m_min{Vector3::ONE * std::numeric_limits<float>::max()},
      m_max{Vector3::ONE * std::numeric_limits<float>::lowest()},
      m_center{(m_max + m_min) * 0.5f}, m_size{m_max - m_min}, m_extent{
                                                                   m_size *
                                                                   0.5f} {};

kirana::math::Bounds3::Bounds3(const Vector3 &point)
    : m_min{point}, m_max{point}, m_center{point}, m_size{Vector3::ZERO},
      m_extent{Vector3::ZERO} {};

kirana::math::Bounds3::Bounds3(const Vector3 &min, const Vector3 &max)
    : m_min{Vector3{std::fmin(min[0], max[0]), std::fmin(min[1], max[1]),
                    std::fmin(min[2], max[2])}},
      m_max{Vector3{std::fmax(min[0], max[0]), std::fmax(min[1], max[1]),
                    std::fmax(min[2], max[2])}},
      m_center{(m_min + m_max) * 0.5f}, m_size{m_max - m_min}, m_extent{
                                                                   m_size *
                                                                   0.5f} {};


kirana::math::Bounds3::Bounds3(const Bounds3 &bounds)
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

kirana::math::Bounds3 &kirana::math::Bounds3::operator=(const Bounds3 &bounds)
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

void kirana::math::Bounds3::encapsulate(const Vector3 &point)
{
    m_min =
        Vector3(std::fmin(m_min[0], point[0]), std::fmin(m_min[1], point[1]),
                std::fmin(m_min[2], point[2]));
    m_max =
        Vector3(std::fmax(m_max[0], point[0]), std::fmax(m_max[1], point[1]),
                std::fmax(m_max[2], point[2]));
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

void kirana::math::Bounds3::encapsulate(const Bounds3 &bounds)
{
    m_min = Vector3(std::fmin(m_min[0], bounds.m_min[0]),
                    std::fmin(m_min[1], bounds.m_min[1]),
                    std::fmin(m_min[2], bounds.m_min[2]));
    m_max = Vector3(std::fmax(m_max[0], bounds.m_max[0]),
                    std::fmax(m_max[1], bounds.m_max[1]),
                    std::fmax(m_max[2], bounds.m_max[2]));
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

void kirana::math::Bounds3::expand(float delta)
{
    m_min -= Vector3::ONE * delta;
    m_max += Vector3::ONE * delta;
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;
    m_extent = m_size * 0.5f;
}

bool kirana::math::Bounds3::contains(const Vector3 &point)
{
    const bool x = (point[0] >= m_min[0]) && (point[0] <= m_max[0]);
    const bool y = (point[1] >= m_min[1]) && (point[1] <= m_max[1]);
    const bool z = (point[2] >= m_min[2]) && (point[2] <= m_max[2]);
    return x && y && z;
}

bool kirana::math::Bounds3::intersectWithRay(const Ray &ray,
                                             Vector3 *enterPoint,
                                             Vector3 *exitPoint)
{
    // Solved by intersecting ray with each plane-slabs. The plane equations
    // are used to solve the intersections.
    // Refer: Section 3.1.2, PBRT 3rd Edition, by Matt Pharr

    float t0 = 0.0f;
    float t1 = ray.getMaxDistance();

    for (size_t i = 0; i < 3; i++)
    {
        const float invRayDir = 1.0f / ray.getDirection()[i];
        float tNear = (m_min[i] - ray.getOrigin()[i]) * invRayDir;
        float tFar = (m_max[i] - ray.getOrigin()[i]) * invRayDir;

        if (tNear > tFar)
            std::swap(tNear, tFar);

        // Ensures robustness in intersection calculation. (Refer PBRT book)
        tFar *= 1.0f + 2.0f * math::gammaf(3);

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)
            return false;
    }

    if (enterPoint != nullptr)
        *enterPoint = ray.getPointAt(t0);
    if (exitPoint != nullptr)
        *exitPoint = ray.getPointAt(t1);
    return true;
}

bool kirana::math::Bounds3::intersectWithRay(const Ray &ray)
{
    // Faster version of ray-box intersection. Checks the entire ray
    // intersection, even in negative direction.
    // Refer: Section 3.1.2, PBRT 3rd Edition, by Matt Pharr

    const Vector3 &rayOrigin = ray.getOrigin();
    const Vector3 invRayDir = 1.0f / ray.getDirection();
    const int dirIsNegative[3] = {invRayDir[0] < 0.0f, invRayDir[1] < 0.0f,
                                  invRayDir[2] < 0.0f};

    float tMin = ((*this)[dirIsNegative[0]][0] - rayOrigin[0]) * invRayDir[0];
    float tMax =
        ((*this)[1 - dirIsNegative[0]][0] - rayOrigin[0]) * invRayDir[0];
    float tYMin = ((*this)[dirIsNegative[1]][1] - rayOrigin[1]) * invRayDir[1];
    float tYMax =
        ((*this)[1 - dirIsNegative[1]][1] - rayOrigin[1]) * invRayDir[1];

    // Ensures robustness in intersection calculation. (Refer PBRT book)
    tMax *= 1.0f + 2.0f * math::gammaf(3);
    tYMax *= 1.0f + 2.0f * math::gammaf(3);

    if (tMin > tYMax || tYMin > tMax)
        return false;
    if (tYMin > tMin)
        tMin = tYMin;
    if (tYMax < tMax)
        tMax = tYMax;

    float tZMin = ((*this)[dirIsNegative[2]][2] - rayOrigin[2]) * invRayDir[2];
    float tZMax =
        ((*this)[1 - dirIsNegative[2]][2] - rayOrigin[2]) * invRayDir[2];

    // Ensures robustness in intersection calculation. (Refer PBRT book)
    tZMax *= 1.0f + 2.0f * math::gammaf(3);

    if (tMin > tZMax || tZMin > tMax)
        return false;
    if (tZMin > tMin)
        tMin = tZMin;
    if (tZMax < tMax)
        tMax = tZMax;

    return (tMin < ray.getMaxDistance()) && (tMax > 0);
}


kirana::math::Vector3 kirana::math::Bounds3::lerp(const Vector3 &t)
{
    return Vector3(math::lerp(m_min[0], m_max[0], t[0]),
                   math::lerp(m_min[1], m_max[1], t[1]),
                   math::lerp(m_min[2], m_max[2], t[2]));
}

kirana::math::Bounds3 kirana::math::Bounds3::createFromCenterSize(
    const Vector3 &center, const Vector3 &size)
{
    Bounds3 bounds;
    bounds.m_center = center;
    bounds.m_size = size;
    bounds.m_extent = bounds.m_size * 0.5;
    bounds.m_max = bounds.m_center + bounds.m_extent;
    bounds.m_min = bounds.m_center - bounds.m_extent;

    return bounds;
}

bool kirana::math::Bounds3::overlaps(const Bounds3 &lhs, const Bounds3 &rhs)
{
    const bool x =
        (lhs.m_max[0] >= rhs.m_min[0]) && (lhs.m_min[0] <= rhs.m_max[0]);
    const bool y =
        (lhs.m_max[1] >= rhs.m_min[1]) && (lhs.m_min[1] <= rhs.m_max[1]);
    const bool z =
        (lhs.m_max[2] >= rhs.m_min[2]) && (lhs.m_min[2] <= rhs.m_max[2]);
    return x && y && z;
}

kirana::math::Bounds3 kirana::math::Bounds3::intersect(const Bounds3 &lhs,
                                                       const Bounds3 &rhs)
{
    return Bounds3(Vector3(std::fmax(lhs.m_min[0], rhs.m_min[0]),
                           std::fmax(lhs.m_min[1], rhs.m_min[1]),
                           std::fmax(lhs.m_min[2], rhs.m_min[2])),
                   Vector3(std::fmin(lhs.m_max[0], rhs.m_max[0]),
                           std::fmin(lhs.m_max[1], rhs.m_max[1]),
                           std::fmin(lhs.m_max[2], rhs.m_max[2])));
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Bounds3 &bounds)
{
    return out << "<Min: " << bounds.m_min << ", Max: " << bounds.m_max
               << ", Center: " << bounds.m_center << ", Size: " << bounds.m_size
               << ", Extent: " << bounds.m_extent << ">";
}