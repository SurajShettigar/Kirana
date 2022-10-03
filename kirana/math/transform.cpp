#include "transform.hpp"

#include "vector3.hpp"
#include "vector4.hpp"
#include "math_utils.hpp"

kirana::math::Transform::Transform() : m_inverse{Matrix4x4::inverse(m_current)}
{
}

kirana::math::Transform::Transform(const Matrix4x4 &mat)
    : m_current{mat}, m_inverse{Matrix4x4::inverse(mat)}
{
}

kirana::math::Transform::Transform(const Matrix4x4 &mat,
                                   const Matrix4x4 &matInverse)
    : m_current{mat}, m_inverse{matInverse}
{
}

kirana::math::Transform::Transform(const Transform &transform)
{
    m_current = transform.m_current;
    m_inverse = transform.m_inverse;
}

kirana::math::Transform &kirana::math::Transform::operator=(
    const Transform &transform)
{
    if (this != &transform)
    {
        m_current = transform.m_current;
        m_inverse = transform.m_inverse;
    }
    return *this;
}

bool kirana::math::Transform::operator==(const Transform &rhs) const
{
    return (*this).m_current == rhs.m_current &&
           (*this).m_inverse == rhs.m_inverse;
}

bool kirana::math::Transform::operator!=(const Transform &rhs) const
{
    return (*this).m_current != rhs.m_current ||
           (*this).m_inverse != rhs.m_inverse;
}

void kirana::math::Transform::translate(
    const kirana::math::Vector3 &translation)
{
    m_current *= Matrix4x4(1.0f, 0.0f, 0.0f, translation.x, 0.0f, 1.0f, 0.0f,
                           translation.y, 0.0f, 0.0f, 1.0f, translation.z, 0.0f,
                           0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(1.0f, 0.0f, 0.0f, -translation.x, 0.0f, 1.0f, 0.0f,
                           -translation.y, 0.0f, 0.0f, 1.0f, -translation.z,
                           0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::rotateX(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f,
                           sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, sin, 0.0f, 0.0f,
                           -sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::rotateY(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin,
                           0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_current *= Matrix4x4(cos, 0.0f, -sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sin,
                           0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::rotateZ(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(cos, sin, 0.0f, 0.0f, -sin, cos, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::rotate(const Vector3 &rotation)
{
    float cosX = std::cos(math::radians(rotation.x));
    float sinX = std::sin(math::radians(rotation.x));
    float cosY = std::cos(math::radians(rotation.y));
    float sinY = std::sin(math::radians(rotation.y));
    float cosZ = std::cos(math::radians(rotation.z));
    float sinZ = std::sin(math::radians(rotation.z));

    // Combined XYZ rotation (euler angles). Same as rotateX() * rotateY() *
    // rotateZ()
    m_current *= Matrix4x4(
        cosY * cosZ, sinX * sinY * cosZ - cosX * sinZ,
        cosX * sinY * cosZ + sinX * sinZ, 0.0f, cosY * sinZ,
        sinX * sinY * sinZ + cosX * cosZ, cosX * sinY * sinZ - sinX * cosZ,
        0.0f, -sinY, sinX * cosY, cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(
        cosY * cosZ, cosY * sinZ, -sinY, 0.0f, sinX * sinY * cosZ - cosX * sinZ,
        sinX * sinY * sinZ + cosX * cosZ, sinX * cosY, 0.0f,
        cosX * sinY * cosZ + sinX * sinZ, cosX * sinY * sinZ - sinX * cosZ,
        cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::rotateAround(float angle, const Vector3 &axis)
{
    // TODO: Implement Rotate Around an axis transform function.
}

void kirana::math::Transform::scale(const Vector3 &scale)
{
    m_current *= Matrix4x4(scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f,
                           0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(1.0f / scale.x, 0.0f, 0.0f, 0.0f, 0.0f,
                           1.0f / scale.y, 0.0f, 0.0f, 0.0f, 0.0f,
                           1.0f / scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

kirana::math::Transform kirana::math::Transform::inverse(
    const Transform &transform)
{
    return Transform(transform.m_inverse, transform.m_current);
}

kirana::math::Transform kirana::math::Transform::transpose(
    const Transform &transform)
{
    return Transform(Matrix4x4::transpose(transform.m_current),
                     Matrix4x4::transpose(transform.m_inverse));
}