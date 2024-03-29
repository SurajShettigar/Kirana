#include "vector4.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "math_utils.hpp"

#include <string>

using kirana::math::Vector4;
using kirana::math::Vector2;
using kirana::math::Vector3;

const Vector4 ZERO{0.0f, 0.0f, 0.0f, 0.0f};
const Vector4 ONE{1.0f, 1.0f, 1.0f, 1.0f};

Vector4::Vector4(std::array<float, 4> vector) : m_current{vector}
{
}

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
        m_current = vec4.m_current;
}

Vector4 &Vector4::operator=(const Vector4 &vec4)
{
    if (this != &vec4)
        m_current = vec4.m_current;
    return *this;
}

bool Vector4::operator==(const Vector4 &rhs) const
{
    return approximatelyEqual(m_current[0], rhs.m_current[0]) &&
           approximatelyEqual(m_current[1], rhs.m_current[1]) &&
           approximatelyEqual(m_current[2], rhs.m_current[2]) &&
           approximatelyEqual(m_current[3], rhs.m_current[3]);
}

bool Vector4::operator!=(const Vector4 &rhs) const
{
    return !(*this == rhs);
}

Vector4::operator Vector2() const
{
    return Vector2(m_current[0], m_current[1]);
}

Vector4::operator Vector3() const
{
    return Vector3(m_current[0], m_current[1], m_current[2]);
}

Vector4::operator std::string() const
{
    return std::string("{") + std::to_string(m_current[0]) + ", " +
           std::to_string(m_current[1]) + ", " + std::to_string(m_current[2]) +
           ", " + std::to_string(m_current[3]) + "}";
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


float Vector4::length() const
{
    return std::sqrt(lengthSquared());
}

float Vector4::lengthSquared() const
{
    return (m_current[0] * m_current[0]) + (m_current[1] * m_current[1]) +
           (m_current[2] * m_current[2]) + (m_current[3] * m_current[3]);
}

void Vector4::normalize()
{
    *this = *this / length();
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
    return out << static_cast<std::string>(vec4);
}