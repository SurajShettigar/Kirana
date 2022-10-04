#include "vector3.hpp"
#include "vector4.hpp"

#include <cmath>
#include <limits>

using kirana::math::Vector3;
using kirana::math::Vector4;

const Vector3 Vector3::ZERO{0.0f, 0.0f, 0.0f};
const Vector3 Vector3::ONE{1.0f, 1.0f, 1.0};
const Vector3 Vector3::UP{0.0f, 1.0f, 0.0f};
const Vector3 Vector3::DOWN{0.0f, -1.0f, 0.0f};
const Vector3 Vector3::LEFT{-1.0f, 0.0f, 0.0f};
const Vector3 Vector3::RIGHT{1.0f, 0.0f, 0.0f};
const Vector3 Vector3::FORWARD{0.0f, 0.0f, -1.0f};
const Vector3 Vector3::BACK{0.0f, 0.0f, 1.0f};

Vector3::Vector3(float x, float y, float z) : m_current{x, y, z}
{
}


Vector3::Vector3(const Vector4 &vec4) : m_current{vec4.x, vec4.y, vec4.z}
{
}

Vector3::Vector3(const Vector3 &vec3)
{
    m_current[0] = vec3[0];
    m_current[1] = vec3[1];
    m_current[2] = vec3[2];
}

Vector3 &Vector3::operator=(const Vector3 &vec3)
{
    if (&vec3 != this)
    {
        m_current[0] = vec3[0];
        m_current[1] = vec3[1];
        m_current[2] = vec3[2];
    }
    return *this;
}

Vector3::operator Vector4() const
{
    return Vector4(m_current[0], m_current[1], m_current[2], 0.0f);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-m_current[0], -m_current[1], -m_current[2]);
}

Vector3 &Vector3::operator+=(const Vector3 &rhs)
{
    m_current[0] += rhs[0];
    m_current[1] += rhs[1];
    m_current[2] += rhs[2];

    return *this;
}
Vector3 &Vector3::operator-=(const Vector3 &rhs)
{
    m_current[0] -= rhs[0];
    m_current[1] -= rhs[1];
    m_current[2] -= rhs[2];
    return *this;
}

Vector3 &Vector3::operator*=(const float rhs)
{
    m_current[0] *= rhs;
    m_current[1] *= rhs;
    m_current[2] *= rhs;
    return *this;
}

Vector3 &Vector3::operator/=(const float rhs)
{
    return *this *= 1 / rhs;
}

bool Vector3::operator==(const Vector3 &rhs) const
{
    return std::fabsf((*this).length() - rhs.length()) <=
           std::numeric_limits<float>::epsilon();
}

bool Vector3::operator!=(const Vector3 &rhs) const
{
    return std::fabsf((*this).length() - rhs.length()) >
           std::numeric_limits<float>::epsilon();
}

float Vector3::length() const
{
    return std::sqrt(lengthSquared());
}

float Vector3::lengthSquared() const
{
    return (m_current[0] * m_current[0]) + (m_current[1] * m_current[1]) +
           (m_current[2] * m_current[2]);
}

Vector3 Vector3::normalize() const
{
    return (*this / length());
}

float Vector3::dot(const Vector3 &v, const Vector3 &w)
{
    return (v[0] * w[0] + v[1] * w[1] + v[2] * w[2]);
}

Vector3 Vector3::cross(const Vector3 &v, const Vector3 &w)
{
    // Static cast to double to ensure there are no rounding errors when two
    // vectors are extremely similar.
    auto vx = static_cast<double>(v[0]);
    auto vy = static_cast<double>(v[1]);
    auto vz = static_cast<double>(v[2]);
    auto wx = static_cast<double>(w[0]);
    auto wy = static_cast<double>(w[1]);
    auto wz = static_cast<double>(w[2]);
    return Vector3(static_cast<float>(vy * wz - vz * wy),
                   static_cast<float>(vz * wx - vx * wz),
                   static_cast<float>(vx * wy - vy * wx));
}

Vector3 Vector3::normalize(const Vector3 &vec3)
{
    return vec3 / vec3.length();
}

Vector3 Vector3::lerp(const Vector3 &v, const Vector3 &w, float t)
{
    return (1 - t) * v + t * w;
}

Vector3 Vector3::reflect(const Vector3 &v, const Vector3 &normal)
{
    return v - 2 * dot(v, normal) * normal;
}

void Vector3::coordinateFrame(const Vector3 &w, Vector3 *u, Vector3 *v)
{
    /// Technique found in :Tom Duff, James Burgess, Per Christensen, Christophe
    /// Hery, Andrew Kensler, Max Liani, and Ryusuke Villemin, Building an
    /// Orthonormal Basis, Revisited, Journal of Computer Graphics Techniques
    /// (JCGT), vol. 6, no. 1, 1-8, 2017
    /// @link {http://jcgt.org/published/0006/01/01/}

    float sign = std::copysignf(1.0f, w.z);
    const float a = -1.0f / (sign + w.z);
    const float b = w.x * w.y * a;

    u->x = 1.0f + sign * w.x * w.x * a;
    u->y = sign * b;
    u->z = -sign * w.x;

    v->x = b;
    v->y = sign + w.y * w.y * a;
    v->z = -w.y;
}

Vector3 kirana::math::operator+(const Vector3 &lhs, const Vector3 &rhs)
{
    return Vector3(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

Vector3 kirana::math::operator-(const Vector3 &lhs, const Vector3 &rhs)
{
    return Vector3(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

Vector3 kirana::math::operator*(float lhs, const Vector3 &rhs)
{
    return Vector3(rhs[0] * lhs, rhs[1] * lhs, rhs[2] * lhs);
}

Vector3 kirana::math::operator*(const Vector3 &lhs, float rhs)
{
    return rhs * lhs;
}

Vector3 kirana::math::operator/(const Vector3 &lhs, float rhs)
{
    return (1 / rhs) * lhs;
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Vector3 &vec3)
{
    return out << '{' << vec3[0] << ", " << vec3[1] << ", " << vec3[2] << '}';
}
