#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "math_utils.hpp"

#include <iostream>

using kirana::math::Vector2;
using kirana::math::Vector3;
using kirana::math::Vector4;

const Vector2 Vector2::ZERO{0.0f, 0.0f};
const Vector2 Vector2::ONE{1.0f, 1.0f};
const Vector2 Vector2::UP{0.0f, 1.0f};
const Vector2 Vector2::DOWN{0.0f, -1.0f};
const Vector2 Vector2::LEFT{-1.0f, 0.0f};
const Vector2 Vector2::RIGHT{1.0f, 0.0f};

Vector2::Vector2(float x, float y) : m_current{x, y}
{
}

Vector2::Vector2(const Vector3 &vec3) : m_current{vec3[0], vec3[1]}
{
}

Vector2::Vector2(const Vector4 &vec4) : m_current{vec4[0], vec4[1]}
{
}

Vector2::Vector2(const Vector2 &vec2)
{
    if (this != &vec2)
    {
        m_current[0] = vec2[0];
        m_current[1] = vec2[1];
    }
}

Vector2 &Vector2::operator=(const Vector2 &vec2)
{
    if (this != &vec2)
    {
        m_current[0] = vec2[0];
        m_current[1] = vec2[1];
    }
    return *this;
}

Vector2::operator Vector3() const
{
    return Vector3(m_current[0], m_current[1], 0.0f);
}

Vector2::operator Vector4() const
{
    return Vector4(m_current[0], m_current[1], 0.0f, 0.0f);
}

Vector2 Vector2::operator-() const
{
    return Vector2(-m_current[0], -m_current[1]);
}

Vector2 &Vector2::operator+=(const Vector2 &rhs)
{
    m_current[0] += rhs[0];
    m_current[1] += rhs[1];
    return *this;
}
Vector2 &Vector2::operator-=(const Vector2 &rhs)
{
    m_current[0] -= rhs[0];
    m_current[1] -= rhs[1];
    return *this;
}

Vector2 &Vector2::operator*=(const float rhs)
{
    m_current[0] *= rhs;
    m_current[1] *= rhs;
    return *this;
}

Vector2 &Vector2::operator/=(const float rhs)
{
    return *this *= 1 / rhs;
}

bool Vector2::operator==(const Vector2 &rhs) const
{
    return approximatelyEqual(m_current[0], rhs.m_current[0]) &&
           approximatelyEqual(m_current[1], rhs.m_current[1]);
}

bool Vector2::operator!=(const Vector2 &rhs) const
{
    return !approximatelyEqual(m_current[0], rhs.m_current[0]) ||
           !approximatelyEqual(m_current[1], rhs.m_current[1]);
}

float Vector2::length() const
{
    return std::sqrt(lengthSquared());
}

float Vector2::lengthSquared() const
{
    return (m_current[0] * m_current[0]) + (m_current[1] * m_current[1]);
}

Vector2 Vector2::normalize() const
{
    return (*this / length());
}

float Vector2::dot(const Vector2 &v, const Vector2 &w)
{
    return (v[0] * w[0] + v[1] * w[1]);
}

Vector2 Vector2::normalize(const Vector2 &vec2)
{
    return vec2 / vec2.length();
}

Vector2 Vector2::lerp(const Vector2 &v, const Vector2 &w, float t)
{
    return (1 - t) * v + t * w;
}

Vector2 kirana::math::operator+(const Vector2 &lhs, const Vector2 &rhs)
{
    return Vector2(lhs[0] + rhs[0], lhs[1] + rhs[1]);
}

Vector2 kirana::math::operator-(const Vector2 &lhs, const Vector2 &rhs)
{
    return Vector2(lhs[0] - rhs[0], lhs[1] - rhs[1]);
}

Vector2 kirana::math::operator*(float lhs, const Vector2 &rhs)
{
    return Vector2(rhs[0] * lhs, rhs[1] * lhs);
}

Vector2 kirana::math::operator*(const Vector2 &lhs, float rhs)
{
    return rhs * lhs;
}

Vector2 kirana::math::operator/(const Vector2 &lhs, float rhs)
{
    return (1 / rhs) * lhs;
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Vector2 &vec2)
{
    return out << '{' << vec2[0] << ", " << vec2[1] << '}';
}
