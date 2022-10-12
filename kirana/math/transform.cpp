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

const kirana::math::Matrix4x4 &kirana::math::Transform::getMatrix(
    bool inverse) const
{
    return inverse ? m_inverse : m_current;
}

kirana::math::Matrix4x4 kirana::math::Transform::getMatrixTransposed(
    bool inverse) const
{
    return inverse ? Matrix4x4::transpose(m_inverse)
                   : Matrix4x4::transpose(m_current);
}

void kirana::math::Transform::translate(
    const kirana::math::Vector3 &translation)
{
    m_current *= Matrix4x4(1.0f, 0.0f, 0.0f, translation[0], 0.0f, 1.0f, 0.0f,
                           translation[1], 0.0f, 0.0f, 1.0f, translation[2],
                           0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(1.0f, 0.0f, 0.0f, -translation[0], 0.0f, 1.0f, 0.0f,
                          -translation[1], 0.0f, 0.0f, 1.0f, -translation[2],
                          0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
}

void kirana::math::Transform::rotateX(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f,
                           sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, sin, 0.0f, 0.0f,
                          -sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
}

void kirana::math::Transform::rotateY(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin,
                           0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(cos, 0.0f, -sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sin,
                          0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
}

void kirana::math::Transform::rotateZ(float angle)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    m_current *= Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(cos, sin, 0.0f, 0.0f, -sin, cos, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
}

void kirana::math::Transform::rotate(const Vector3 &rotation)
{
    float cosX = std::cos(math::radians(rotation[0]));
    float sinX = std::sin(math::radians(rotation[0]));
    float cosY = std::cos(math::radians(rotation[1]));
    float sinY = std::sin(math::radians(rotation[1]));
    float cosZ = std::cos(math::radians(rotation[2]));
    float sinZ = std::sin(math::radians(rotation[2]));

    // Combined XYZ rotation (euler angles). Same as rotateX() * rotateY() *
    // rotateZ()
    m_current *= Matrix4x4(
        cosY * cosZ, sinX * sinY * cosZ - cosX * sinZ,
        cosX * sinY * cosZ + sinX * sinZ, 0.0f, cosY * sinZ,
        sinX * sinY * sinZ + cosX * cosZ, cosX * sinY * sinZ - sinX * cosZ,
        0.0f, -sinY, sinX * cosY, cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(cosY * cosZ, cosY * sinZ, -sinY, 0.0f,
                          sinX * sinY * cosZ - cosX * sinZ,
                          sinX * sinY * sinZ + cosX * cosZ, sinX * cosY, 0.0f,
                          cosX * sinY * cosZ + sinX * sinZ,
                          cosX * sinY * sinZ - sinX * cosZ, cosX * cosY, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
}

void kirana::math::Transform::rotateAround(float angle, const Vector3 &a)
{
    // Goldman, Ronald, “Matrices and Transformations,” in Andrew S. Glassner,
    // ed., Graphics Gems, Academic Press, pp. 472–475, 1990. Cited on p. 75
    // @link {https://dl.acm.org/doi/10.5555/90767.90908}

    float c = std::cos(math::radians(angle));
    float omc = 1.0f - c;
    float s = std::sin(math::radians(angle));
    m_current *= Matrix4x4(
        c + omc * a[0] * a[0], omc * a[0] * a[1] - a[2] * s,
        omc * a[0] * a[2] + a[1] * s, 0.0f, omc * a[0] * a[1] + a[2] * s,
        c + omc * a[1] * a[1], omc * a[1] * a[2] - a[0] * s, 0.0f,
        omc * a[0] * a[2] - a[1] * s, omc * a[1] * a[2] + a[0] * s,
        c + omc * a[2] * a[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    m_inverse =
        Matrix4x4(c + omc * a[0] * a[0], omc * a[0] * a[1] + a[2] * s,
                  omc * a[0] * a[2] - a[1] * s, 0.0f,
                  omc * a[0] * a[1] - a[2] * s, c + omc * a[1] * a[1],
                  omc * a[1] * a[2] + a[0] * s, 0.0f,
                  omc * a[0] * a[2] + a[1] * s, omc * a[1] * a[2] - a[0] * s,
                  c + omc * a[2] * a[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
        m_inverse;
}

void kirana::math::Transform::lookAt(const Vector3 &lookAtPos,
                                     const Vector3 &up)
{
    Vector3 pos(m_current[0][3] / m_current[3][3],
                m_current[1][3] / m_current[3][3],
                m_current[2][3] / m_current[3][3]);
    Vector3 z = -((lookAtPos - pos).normalize());
    Vector3 x = Vector3::cross(up.normalize(), z);
    Vector3 y = Vector3::cross(z, x);

    // The above calculated basis vectors are camera's basis in world space
    // (Camera's transform matrix). In order to get view matrix, we just invert
    // the matrix one would obtain from the above basis vectors. Look at
    // https://www.3dgep.com/understanding-the-view-matrix/ for a detailed
    // explanation.

    // m_current is now a view-matrix.
    m_current = Matrix4x4(x[0], x[1], x[2], Vector3::dot(x, pos), y[0], y[1],
                          y[2], Vector3::dot(y, pos), z[0], z[1], z[2],
                          -Vector3::dot(z, pos), 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(x[0], y[0], z[0], -Vector3::dot(x, pos), x[1], y[1],
                          z[1], -Vector3::dot(y, pos), x[2], y[2], z[2],
                          Vector3::dot(z, pos), 0.0f, 0.0f, 0.0f, 1.0f);
}

void kirana::math::Transform::scale(const Vector3 &scale)
{
    m_current *=
        Matrix4x4(scale[0], 0.0f, 0.0f, 0.0f, 0.0f, scale[1], 0.0f, 0.0f, 0.0f,
                  0.0f, scale[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    m_inverse = Matrix4x4(1.0f / scale[0], 0.0f, 0.0f, 0.0f, 0.0f,
                          1.0f / scale[1], 0.0f, 0.0f, 0.0f, 0.0f,
                          1.0f / scale[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_inverse;
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
    float left, float right, float bottom, float top, float near, float far,
    bool graphicsAPI, bool flipY)
{
    float nf = graphicsAPI ? (far - near) : (near - far);

    Transform transform;
    transform.m_current = Matrix4x4(
        2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
        0.0f, (flipY ? -1.0f : 0.0f) * 2.0f / (top - bottom), 0.0f,
        -(top + bottom) / (top - bottom), 0.0f, 0.0f,
        (graphicsAPI ? -1.0f : 1.0f) * 2.0f / nf, -(near + far) / nf, 0.0f,
        0.0f, 0.0f, 1.0f);

    transform.m_inverse =
        Matrix4x4(0.5f * (right - left), 0.0f, 0.0f, 0.5f * (right + left),
                  0.0f, (flipY ? -1.0f : 0.0f) * 0.5f * (top - bottom), 0.0f,
                  0.5f * (top + bottom), 0.0f, 0.0f,
                  (graphicsAPI ? -1.0f : 1.0f) * 0.5f * nf,
                  (graphicsAPI ? -1.0f : 1.0f) * 0.5f * (near + far), 0.0f,
                  0.0f, 0.0f, 1.0f);
    return transform;
}

kirana::math::Transform kirana::math::Transform::getOrthographicTransform(
    float size, float aspectRatio, float near, float far, bool graphicsAPI,
    bool flipY)
{
    return getOrthographicTransform(
        -size / 2.0f, size / 2.0f, (-size / 2.0f) / aspectRatio,
        (size / 2.0f) / aspectRatio, near, far, graphicsAPI, flipY);
}

kirana::math::Transform kirana::math::Transform::getPerspectiveTransform(
    float fov, float aspectRatio, float near, float far, bool graphicsAPI,
    bool flipY)
{
    float top = std::abs(near) * std::tan(math::radians(fov * 0.5f));
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;

    float lr = graphicsAPI ? right - left : left - right;
    float bt = graphicsAPI ? top - bottom : bottom - top;
    float nf = graphicsAPI ? far - near : near - far;

    Transform transform;
    transform.m_current = Matrix4x4(
        2.0f * near / (right - left), 0.0f, (left + right) / lr, 0.0f, 0.0f,
        (flipY ? -1.0f : 1.0f) * 2.0f * near / (top - bottom),
        (bottom + top) / bt, 0.0f, 0.0f, 0.0f,
        (graphicsAPI ? -1.0f : 1.0f) * (far + near) / nf,
        (graphicsAPI ? -1.0f : 1.0f) * 2.0f * far * near / (far - near), 0.0f,
        0.0f, (graphicsAPI ? -1.0f : 1.0f), 0.0f);

    transform.m_inverse = Matrix4x4(
        0.5f * far * (right - left), 0.0f, 0.0f, 0.5f * far * (left + right),
        0.0f, (flipY ? -1.0f : 1.0f) * 0.5f * far * (top - bottom), 0.0f,
        0.5f * far * (bottom + top), 0.0f, 0.0f, 0.0f, far * near, 0.0f, 0.0f,
        0.5f * (far - near), 0.5f * (-far - near) + far + near);
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