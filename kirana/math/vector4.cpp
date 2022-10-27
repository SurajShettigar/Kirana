#include "vector4.hpp"
#include "vector2.hpp"
#include "vector3.hpp"

#include <cmath>
#include <limits>
#include <iostream>

using kirana::math::Vector2;
using kirana::math::Vector3;
using kirana::math::Vector4;

const Vector4 ZERO{0.0f, 0.0f, 0.0f, 0.0f};
const Vector4 ONE{1.0f, 1.0f, 1.0f, 1.0f};

Vector4::Vector4(float x, float y, float z, float w) : m_current{x, y, z, w}
{
}

Vector4::Vector4(const Vector2 &vec2, float z, float w)
    : m_current{vec2[0], vec2[1], z, w}
{
}

Vector4::Vector4(const Vector3 &vec3, float w)
    : m_current{vec3[0], vec3[1], vec3[2], w}
{
}

Vector4::Vector4(const Vector4 &vec4)
{
    if (this != &vec4)
    {
        m_current[0] = vec4[0];
        m_current[1] = vec4[1];
        m_current[2] = vec4[2];
        m_current[3] = vec4[3];
    }
}

Vector4 &Vector4::operator=(const Vector4 &vec4)
{
    if (this != &vec4)
    {
        m_current[0] = vec4[0];
        m_current[1] = vec4[1];
        m_current[2] = vec4[2];
        m_current[3] = vec4[3];
    }
    return *this;
}

Vector4::operator Vector2() const
{
    return Vector2(m_current[0], m_current[1]);
}

Vector4::operator Vector3() const
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