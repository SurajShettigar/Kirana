#include "vector4.hpp"
#include "vector3.hpp"

#include <cmath>
#include <limits>

using kirana::math::Vector4;

Vector4::Vector4(float x, float y, float z, float w) : m_current{x, y, z, w}
{
}
Vector4::Vector4(const Vector3 &vec3, float w)
    : m_current{vec3.x, vec3.y, vec3.z, w}
{
}

Vector4::Vector4(const Vector4 &w)
{
    m_current[0] = w[0];
    m_current[1] = w[1];
    m_current[2] = w[2];
    m_current[3] = w[3];
}

Vector4 &Vector4::operator=(const Vector4 &vec4)
{
    if (&vec4 != this)
    {
        m_current[0] = vec4[0];
        m_current[1] = vec4[1];
        m_current[2] = vec4[2];
        m_current[3] = vec4[3];
    }
    return *this;
}

Vector4::operator Vector3()
{
    return Vector3(m_current[0], m_current[1], m_current[2]);
}

Vector4 Vector4::operator-() const
{
    return Vector4(-m_current[0], -m_current[1], -m_current[2], -m_current[3]);
}

Vector4 &Vector4::operator+=(const Vector4 &rhs)
{
    m_current[0] += rhs[0];
    m_current[1] += rhs[1];
    m_current[2] += rhs[2];
    m_current[3] += rhs[3];

    return *this;
}
Vector4 &Vector4::operator-=(const Vector4 &rhs)
{
    m_current[0] -= rhs[0];
    m_current[1] -= rhs[1];
    m_current[2] -= rhs[2];
    m_current[3] -= rhs[3];
    return *this;
}

Vector4 &Vector4::operator*=(const float rhs)
{
    m_current[0] *= rhs;
    m_current[1] *= rhs;
    m_current[2] *= rhs;
    m_current[3] *= rhs;
    return *this;
}

Vector4 &Vector4::operator/=(const float rhs)
{
    return *this *= 1 / rhs;
}

bool Vector4::operator==(const Vector4 &rhs) const
{
    return std::fabsf((*this).length() - rhs.length()) <=
           std::numeric_limits<float>::epsilon();
}

bool Vector4::operator!=(const Vector4 &rhs) const
{
    return std::fabsf((*this).length() - rhs.length()) >
           std::numeric_limits<float>::epsilon();
}


float Vector4::length() const
{
    return std::sqrt(lengthSquared());
}

float Vector4::lengthSquared() const
{
    return (m_current[0] * m_current[0]) + (m_current[1] * m_current[1]) +
           (m_current[2] * m_current[2]) + (m_current[3] * m_current[3]);
}

Vector4 Vector4::normalize() const
{
    return (*this / length());
}

Vector4 Vector4::normalize(const Vector4 &vec4)
{
    return vec4 / vec4.length();
}

Vector4 Vector4::lerp(const Vector4 &v, const Vector4 &w, float t)
{
    return (1 - t) * v + t * w;
}


Vector4 kirana::math::operator+(const Vector4 &lhs, const Vector4 &rhs)
{
    return Vector4(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2],
                   lhs[3] + rhs[3]);
}

Vector4 kirana::math::operator-(const Vector4 &lhs, const Vector4 &rhs)
{
    return Vector4(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2],
                   lhs[3] - rhs[3]);
}

Vector4 kirana::math::operator*(float lhs, const Vector4 &rhs)
{
    return Vector4(rhs[0] * lhs, rhs[1] * lhs, rhs[2] * lhs, rhs[3] * lhs);
}

Vector4 kirana::math::operator*(const Vector4 &lhs, float rhs)
{
    return rhs * lhs;
}

Vector4 kirana::math::operator/(const Vector4 &lhs, float rhs)
{
    return (1 / rhs) * lhs;
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Vector4 &vec4)
{
    return out << '{' << vec4[0] << ", " << vec4[1] << ", " << vec4[2] << ", "
               << vec4[3] << '}';
}