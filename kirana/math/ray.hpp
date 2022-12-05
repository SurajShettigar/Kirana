#ifndef RAY_HPP
#define RAY_HPP

#include "vector3.hpp"
#include <limits>

namespace kirana::math
{
class Ray
{
  private:
    Vector3 m_origin;
    Vector3 m_direction;
    Vector3 m_invDirection;
    mutable float m_tMax;

  public:
    Ray(const Vector3 &origin, const Vector3 &direction,
        float maxDistance = std::numeric_limits<float>::max())
        : m_origin{origin}, m_direction{direction}, m_tMax{maxDistance} {};
    ~Ray() = default;

    Ray(const Ray &ray)
    {
        if (this != &ray)
        {
            m_origin = ray.m_origin;
            m_direction = ray.m_direction;
            m_tMax = ray.m_tMax;
        }
    }

    Ray &operator=(const Ray &ray)
    {
        if (this != &ray)
        {
            m_origin = ray.m_origin;
            m_direction = ray.m_direction;
            m_tMax = ray.m_tMax;
        }
        return *this;
    }

    bool operator==(const Ray &rhs)
    {
        return m_origin == rhs.m_origin && m_direction == rhs.m_direction;
    }
    bool operator!=(const Ray &rhs)
    {
        return m_origin != rhs.m_origin || m_direction != rhs.m_direction;
    }

    explicit operator std::string() const
    {
        return std::string("<Origin: ") + std::string(m_origin) +
               ", Direction: " + std::string(m_direction) + ">";
    }

    // Getters-Setters
    inline Vector3 getOrigin() const
    {
        return m_origin;
    }

    inline Vector3 getDirection() const
    {
        return m_direction;
    }

    inline float getMaxDistance() const
    {
        return m_tMax;
    }

    inline void setMaxDistance(float max) const
    {
        m_tMax = max;
    }

    inline Vector3 getPointAt(float t) const
    {
        return m_origin + t * m_direction;
    }

    Vector3 operator()(float t) const
    {
        return getPointAt(t);
    }

    friend std::ostream &operator<<(std::ostream &out, const Ray &ray)
    {
        return out << "<Origin: " << ray.m_origin
                   << ", Direction: " << ray.m_direction << ">";
    }
};
} // namespace kirana::math
#endif