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


kirana::math::Transform &kirana::math::Transform::operator*=(
    const Transform &rhs)
{
    this->m_current *= rhs.m_current;
    this->m_inverse *= rhs.m_inverse;
    return *this;
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

void kirana::math::Transform::rotateAround(float angle, const Vector3 &a)
{
    // Goldman, Ronald, “Matrices and Transformations,” in Andrew S. Glassner,
    // ed., Graphics Gems, Academic Press, pp. 472–475, 1990. Cited on p. 75
    // @link {https://dl.acm.org/doi/10.5555/90767.90908}

    float c = std::cos(math::radians(angle));
    float omc = 1.0f - c;
    float s = std::sin(math::radians(angle));

    m_current *=
        Matrix4x4(c + omc * a.x * a.x, omc * a.x * a.y - a.z * s,
                  omc * a.x * a.z + a.y * s, 0.0f, omc * a.x * a.y + a.z * s,
                  c + omc * a.y * a.y, omc * a.y * a.z - a.x * s, 0.0f,
                  omc * a.x * a.z - a.y * s, omc * a.y * a.z + a.x * s,
                  c + omc * a.z * a.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    m_inverse *=
        Matrix4x4(c + omc * a.x * a.x, omc * a.x * a.y + a.z * s,
                  omc * a.x * a.z - a.y * s, 0.0f, omc * a.x * a.y - a.z * s,
                  c + omc * a.y * a.y, omc * a.y * a.z + a.x * s, 0.0f,
                  omc * a.x * a.z + a.y * s, omc * a.y * a.z - a.x * s,
                  c + omc * a.z * a.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::lookAt(const Vector3 &lookAtPos,
                                     const Vector3 &up)
{
    Vector3 z = -(lookAtPos - static_cast<Vector3>(m_current[3])).normalize();
    Vector3 x = Vector3::cross(up.normalize(), z);
    Vector3 y = Vector3::cross(z, x);

    // The above calculated basis vectors are camera's basis in world space
    // (Camera's transform matrix). In order to get view matrix, we just invert
    // the matrix one would obtain from the above basis vectors. Look at
    // https://www.3dgep.com/understanding-the-view-matrix/ for a detailed
    // explanation.

    // m_current is now a view-matrix.
    m_current *= Matrix4x4(x.x, x.y, x.z, 0.0f, y.x, y.y, y.z, 0.0f, z.x, z.y,
                           z.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse *= Matrix4x4(x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z,
                           z.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
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

kirana::math::Transform kirana::math::Transform::getOrthographicTransform(
    float left, float right, float bottom, float top, float near, float far)
{
    Transform transform;
    transform.m_current = Matrix4x4(
        2.0f / (right - left), 0.0f, 0.0f, -((right + left) / (right - left)),
        0.0f, 2.0f / (top - bottom), 0.0f, -((top + bottom) / (top - bottom)),
        0.0f, 0.0f, 2.0f / (near - far), -((near + far) / (near - far)), 0.0f,
        0.0f, 0.0f, 1.0f);

    transform.m_inverse = Matrix4x4(
        0.5f * (right - left), 0.0f, 0.0f, 0.5f * (right + left), 0.0f,
        0.5f * (top - bottom), 0.0f, 0.5f * (top + bottom), 0.0f, 0.0f,
        0.5f * (near - far), 0.5f * (near + far), 0.0f, 0.0f, 0.0f, 1.0f);
    return transform;
}

kirana::math::Transform kirana::math::Transform::getOrthographicTransform(
    float size, float aspectRatio, float near, float far)
{
    return getOrthographicTransform(-size / 2.0f, size / 2.0f,
                                    (-size / 2.0f) / aspectRatio,
                                    (size / 2.0f) / aspectRatio, near, far);
}

kirana::math::Transform kirana::math::Transform::getPerspectiveTransform(
    float fov, float aspectRatio, float near, float far)
{
    float top = near * std::tan(math::radians(fov * 0.5f));
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;

    Transform transform;
    transform.m_current =
        Matrix4x4(2.0f * near / (right - left), 0.0f,
                  (left + right) / (left - right), 0.0f, 0.0f,
                  2.0f * near / (top - bottom), (bottom + top) / (bottom - top),
                  0.0f, 0.0f, 0.0f, (far + near) / (near - far),
                  2.0f * far * near / (far - near), 0.0f, 0.0f, 1.0f, 0.0f);

    transform.m_inverse = Matrix4x4(
        0.5f * far * (right - left), 0.0f, 0.0f, 0.5f * far * (left + right),
        0.0f, 0.5f * far * (top - bottom), 0.0f, 0.5f * far * (bottom + top),
        0.0f, 0.0f, 0.0f, far * near, 0.0f, 0.0f, 0.5f * (far - near),
        0.5f * (-far - near) + far + near);
    return transform;
}

kirana::math::Transform kirana::math::operator*(const Transform &lhs,
                                                const Transform &rhs)
{
    Transform transform;
    transform.m_current = lhs.m_current * rhs.m_current;
    transform.m_inverse = lhs.m_inverse * rhs.m_inverse;
    return transform;
}

kirana::math::Vector4 kirana::math::operator*(const Transform &lhs,
                                              const Vector4 &rhs)
{
    return lhs.m_current * rhs;
}