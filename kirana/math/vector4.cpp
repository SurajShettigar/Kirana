#include "vector4.hpp"
#include "vector3.hpp"

#include <cmath>
#include <limits>

using kirana::math::Vector4;

Vector4::Vector4(float x, float y, float z, float w) : v{x, y, z, w}
{
}
Vector4::Vector4(const Vector3 &vec3, float w) : v{vec3.x, vec3.y, vec3.z, w}
{
}

Vector4::Vector4(const Vector4 &w)
{
    v[0] = w[0];
    v[1] = w[1];
    v[2] = w[2];
    v[3] = w[3];
}

Vector4 &Vector4::operator=(const Vector4 &w)
{
    if (&w != this)
    {
        v[0] = w[0];
        v[1] = w[1];
        v[2] = w[2];
        v[3] = w[3];
    }
    return *this;
}

Vector4::operator Vector3()
{
    return Vector3(v[0], v[1], v[2]);
}

Vector4 Vector4::operator-() const
{
    return Vector4(-v[0], -v[1], -v[2], -v[3]);
}

Vector4 &Vector4::operator+=(const Vector4 &w)
{
    v[0] += w[0];
    v[1] += w[1];
    v[2] += w[2];
    v[3] += w[3];

    return *this;
}
Vector4 &Vector4::operator-=(const Vector4 &w)
{
    v[0] -= w[0];
    v[1] -= w[1];
    v[2] -= w[2];
    v[3] -= w[3];
    return *this;
}

Vector4 &Vector4::operator*=(const float a)
{
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
    v[3] *= a;
    return *this;
}

Vector4 &Vector4::operator/=(const float a)
{
    return *this *= 1 / a;
}

float Vector4::length() const
{
    return std::sqrt(lengthSquared());
}

float Vector4::lengthSquared() const
{
    return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]);
}

Vector4 Vector4::normalize() const
{
    return (*this / length());
}

Vector4 Vector4::normalize(const Vector4 &v)
{
    return v / v.length();
}

Vector4 Vector4::lerp(const Vector4 &v, const Vector4 &w, float t)
{
    return (1 - t) * v + t * w;
}


Vector4 kirana::math::operator+(const Vector4 &v, const Vector4 &w)
{
    return Vector4(v[0] + w[0], v[1] + w[1], v[2] + w[2], v[3] + w[3]);
}

Vector4 kirana::math::operator-(const Vector4 &v, const Vector4 &w)
{
    return Vector4(v[0] - w[0], v[1] - w[1], v[2] - w[2], v[3] - w[3]);
}

Vector4 kirana::math::operator*(float a, const Vector4 &v)
{
    return Vector4(v[0] * a, v[1] * a, v[2] * a, v[3] * a);
}

Vector4 kirana::math::operator*(const Vector4 &v, float a)
{
    return a * v;
}

Vector4 kirana::math::operator/(const Vector4 &v, float a)
{
    return (1 / a) * v;
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Vector3 &v)
{
    return out << '{' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3]
               << '}';
}

bool kirana::math::operator==(const Vector4 &v, const Vector4 &w)
{
    return std::fabsf(v.length() - w.length()) <=
           std::numeric_limits<float>::epsilon();
}

bool kirana::math::operator!=(const Vector4 &v, const Vector4 &w)
{
    return std::fabsf(v.length() - w.length()) >
           std::numeric_limits<float>::epsilon();
}
