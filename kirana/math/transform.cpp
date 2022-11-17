#include "transform.hpp"

#include "vector3.hpp"
#include "vector4.hpp"
#include "math_utils.hpp"


void kirana::math::Transform::calculateLocalMatrix()
{
    m_localMatrix = Matrix4x4::translationMatrix(m_localPosition) *
                    Matrix4x4::rotationMatrix(m_localRotation) *
                    Matrix4x4::scaleMatrix(m_localScale);
}


kirana::math::Matrix4x4 kirana::math::Transform::getParentMatrix() const
{
    if (m_parent != nullptr && m_parent != this)
        return m_parent->getMatrix();
    return Matrix4x4::IDENTITY;
}

kirana::math::Matrix4x4 kirana::math::Transform::getGlobalMatrix() const
{
    if (m_parent != nullptr && m_parent != this)
        return m_parent->getMatrix() * m_localMatrix;
    return m_localMatrix;
}

kirana::math::Transform::Transform(Transform *parent)
    : m_parent{parent}, m_localMatrix{Matrix4x4::IDENTITY}, m_localPosition{},
      m_localRotation{}, m_localScale{Vector3::ONE}
{
}

kirana::math::Transform::Transform(const Matrix4x4 &mat, Transform *parent)
    : m_parent{parent}, m_localMatrix{mat}
{
    Matrix4x4::decomposeMatrix(m_localMatrix, &m_localPosition, &m_localScale,
                               nullptr, &m_localRotation);
}


kirana::math::Transform::Transform(const Transform &transform)
{
    if (this != &transform)
    {
        m_parent = transform.m_parent;
        m_localMatrix = transform.m_localMatrix;
        m_localPosition = transform.m_localPosition;
        m_localRotation = transform.m_localRotation;
        m_localScale = transform.m_localScale;
    }
}

kirana::math::Transform &kirana::math::Transform::operator=(
    const Transform &transform)
{
    if (this != &transform)
    {
        m_parent = transform.m_parent;
        m_localMatrix = transform.m_localMatrix;
        m_localPosition = transform.m_localPosition;
        m_localRotation = transform.m_localRotation;
        m_localScale = transform.m_localScale;
    }
    return *this;
}

bool kirana::math::Transform::operator==(const Transform &rhs) const
{
    return (*this).m_localMatrix == rhs.m_localMatrix &&
           m_parent == rhs.m_parent;
}

bool kirana::math::Transform::operator!=(const Transform &rhs) const
{
    return (*this).m_localMatrix != rhs.m_localMatrix &&
           m_parent != rhs.m_parent;
}

kirana::math::Matrix4x4 kirana::math::Transform::getMatrix(Space space) const
{
    if (space == Space::World)
        return getGlobalMatrix();
    return m_localMatrix;
}

kirana::math::Vector3 kirana::math::Transform::getRight(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getGlobalMatrix();
        return Vector3(globalMat[0][0], globalMat[1][0], globalMat[2][0]);
    }
    return Vector3(m_localMatrix[0][0], m_localMatrix[1][0],
                   m_localMatrix[2][0]);
}

kirana::math::Vector3 kirana::math::Transform::getUp(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getGlobalMatrix();
        return Vector3(globalMat[0][1], globalMat[1][1], globalMat[2][1]);
    }
    return Vector3(m_localMatrix[0][1], m_localMatrix[1][1],
                   m_localMatrix[2][1]);
}

kirana::math::Vector3 kirana::math::Transform::getForward(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getGlobalMatrix();
        return Vector3(globalMat[0][2], globalMat[1][2], globalMat[2][2]);
    }
    return Vector3(m_localMatrix[0][2], m_localMatrix[1][2],
                   m_localMatrix[2][2]);
}

kirana::math::Vector3 kirana::math::Transform::getPosition(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getGlobalMatrix();
        return Vector3(globalMat[0][3], globalMat[1][3], globalMat[2][3]);
    }
    return m_localPosition;
}

kirana::math::Vector3 kirana::math::Transform::getRotation(Space space) const
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            Vector3 rot;
            Matrix4x4::decomposeMatrix(getGlobalMatrix(), nullptr, nullptr,
                                       nullptr, &rot);
            return rot;
        }
    }
    return m_localRotation;
}

kirana::math::Vector3 kirana::math::Transform::getScale(Space space) const
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            Vector3 scale;
            Matrix4x4::decomposeMatrix(getGlobalMatrix(), nullptr, &scale,
                                       nullptr, nullptr);
            return scale;
        }
    }
    return m_localScale;
}

void kirana::math::Transform::setPosition(const Vector3 &position, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localPosition =
                static_cast<Vector3>(Matrix4x4::inverse(getParentMatrix()) *
                                     Vector4(position, 1.0f));
        }
        else
            m_localPosition = position;
    }
    else
        m_localPosition = position;
    calculateLocalMatrix();
}

void kirana::math::Transform::setRotation(const Vector3 &rotation, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate Local rotation from global
            m_localRotation = rotation;
        }
        else
            m_localRotation = rotation;
    }
    else
        m_localRotation = rotation;
    calculateLocalMatrix();
}

void kirana::math::Transform::setLocalScale(const Vector3 &scale)
{
    m_localScale = scale;
    calculateLocalMatrix();
}


void kirana::math::Transform::translate(
    const kirana::math::Vector3 &translation, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localMatrix = Matrix4x4::inverse(getParentMatrix()) *
                            Matrix4x4::translationMatrix(translation) *
                            m_parent->getMatrix() * m_localMatrix;
            m_localPosition[0] = m_localMatrix[0][3];
            m_localPosition[1] = m_localMatrix[1][3];
            m_localPosition[2] = m_localMatrix[2][3];
        }
        else
            m_localMatrix =
                Matrix4x4::translationMatrix(translation) * m_localMatrix;
    }
    else
        m_localMatrix =
            Matrix4x4::translationMatrix(translation) * m_localMatrix;

    m_localPosition[0] = m_localMatrix[0][3];
    m_localPosition[1] = m_localMatrix[1][3];
    m_localPosition[2] = m_localMatrix[2][3];
}

void kirana::math::Transform::rotateX(float angle, Space space)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));

    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate local matrix from global x-axis rotation.
            // TODO: Replace by rotateX static Matrix4x4 function.
            m_localMatrix =
                Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f,
                          sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
        }
        else
            m_localMatrix =
                Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f,
                          sin, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
    }
    else
        m_localMatrix =
            Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f, sin,
                      cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
            m_localMatrix;

    // TODO: Calculate local x-angle from global x-angle.
    m_localRotation[0] += angle;
    m_localRotation[0] = math::clampEulerAngle(m_localRotation[0]);
}

void kirana::math::Transform::rotateY(float angle, Space space)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate local matrix from global y-axis rotation.
            // TODO: Replace by rotateY static Matrix4x4 function.
            m_localMatrix =
                Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin,
                          0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
        }
        else
            m_localMatrix =
                Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin,
                          0.0f, cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
    }
    else
        m_localMatrix =
            Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin, 0.0f,
                      cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
            m_localMatrix;

    // TODO: Calculate local y-angle from global y-angle.
    m_localRotation[1] += angle;
    m_localRotation[1] = math::clampEulerAngle(m_localRotation[1]);
}

void kirana::math::Transform::rotateZ(float angle, Space space)
{
    float cos = std::cos(math::radians(angle));
    float sin = std::sin(math::radians(angle));
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate local matrix from global Z-axis rotation.
            // TODO: Replace by rotateZ static Matrix4x4 function.
            m_localMatrix =
                Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
        }
        else
            m_localMatrix =
                Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
                m_localMatrix;
    }
    else
        m_localMatrix =
            Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f, 0.0f,
                      1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
            m_localMatrix;

    // TODO: Calculate local Z-angle from global z-angle.
    m_localRotation[2] += angle;
    m_localRotation[2] = math::clampEulerAngle(m_localRotation[2]);
}

void kirana::math::Transform::rotate(const Vector3 &rotation, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate local matrix from global rotation matrix.
            m_localMatrix = Matrix4x4::rotationMatrix(rotation) * m_localMatrix;
        }
        else
            m_localMatrix = Matrix4x4::rotationMatrix(rotation) * m_localMatrix;
    }
    else
        m_localMatrix = Matrix4x4::rotationMatrix(rotation) * m_localMatrix;

    // TODO: Calculate local rotation from global rotation.
    m_localRotation += rotation;
    m_localRotation[0] = math::clampEulerAngle(m_localRotation[0]);
    m_localRotation[1] = math::clampEulerAngle(m_localRotation[1]);
    m_localRotation[2] = math::clampEulerAngle(m_localRotation[2]);
}

void kirana::math::Transform::rotateAround(float angle, const Vector3 &a,
                                           Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            // TODO: Calculate local matrix from global rotation matrix.
            m_localMatrix = Matrix4x4::rotationMatrix(a, angle) * m_localMatrix;
        }
        else
            m_localMatrix = Matrix4x4::rotationMatrix(a, angle) * m_localMatrix;
    }
    else
        m_localMatrix = Matrix4x4::rotationMatrix(a, angle) * m_localMatrix;

    // TODO: Calculate local Z-angle from global z-angle.
    m_localRotation[0] += angle;
    m_localRotation[1] += angle;
    m_localRotation[2] += angle;
    m_localRotation[0] = math::clampEulerAngle(m_localRotation[0]);
    m_localRotation[1] = math::clampEulerAngle(m_localRotation[1]);
    m_localRotation[2] = math::clampEulerAngle(m_localRotation[2]);
}

void kirana::math::Transform::lookAt(const Vector3 &lookAtPos,
                                     const Vector3 &up, Space space)
{
    // TODO: Implement World space lookAt.
    Vector3 z = -((lookAtPos - m_localPosition).normalize());
    Vector3 x = Vector3::cross(up.normalize(), z);
    Vector3 y = Vector3::cross(z, x);

    // The above calculated basis vectors are camera's basis in world space
    // (Camera's transform matrix). In order to get view matrix, we just invert
    // the matrix one would obtain from the above basis vectors. Look at
    // https://www.3dgep.com/understanding-the-view-matrix/ for a detailed
    // explanation.

    // m_localMatrix is now a view-matrix.
    m_localMatrix = Matrix4x4(
        x[0], x[1], x[2], Vector3::dot(x, m_localPosition), y[0], y[1], y[2],
        Vector3::dot(y, m_localPosition), z[0], z[1], z[2],
        -Vector3::dot(z, m_localPosition), 0.0f, 0.0f, 0.0f, 1.0f);
}

kirana::math::Transform kirana::math::Transform::getOrthographicTransform(
    float left, float right, float bottom, float top, float near, float far,
    bool graphicsAPI, bool flipY)
{
    float nf = graphicsAPI ? (far - near) : (near - far);

    Transform transform;
    transform.m_localMatrix = Matrix4x4(
        2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
        0.0f, (flipY ? -1.0f : 0.0f) * 2.0f / (top - bottom), 0.0f,
        -(top + bottom) / (top - bottom), 0.0f, 0.0f,
        (graphicsAPI ? -1.0f : 1.0f) * 2.0f / nf, -(near + far) / nf, 0.0f,
        0.0f, 0.0f, 1.0f);

    //    transform.m_inverse =
    //        Matrix4x4(0.5f * (right - left), 0.0f, 0.0f, 0.5f * (right +
    //        left),
    //                  0.0f, (flipY ? -1.0f : 0.0f) * 0.5f * (top - bottom),
    //                  0.0f, 0.5f * (top + bottom), 0.0f, 0.0f, (graphicsAPI ?
    //                  -1.0f : 1.0f) * 0.5f * nf, (graphicsAPI ? -1.0f : 1.0f)
    //                  * 0.5f * (near + far), 0.0f, 0.0f, 0.0f, 1.0f);
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
    transform.m_localMatrix = Matrix4x4(
        2.0f * near / (right - left), 0.0f, (left + right) / lr, 0.0f, 0.0f,
        (flipY ? -1.0f : 1.0f) * 2.0f * near / (top - bottom),
        (bottom + top) / bt, 0.0f, 0.0f, 0.0f,
        (graphicsAPI ? -1.0f : 1.0f) * (far + near) / nf,
        (graphicsAPI ? -1.0f : 1.0f) * 2.0f * far * near / (far - near), 0.0f,
        0.0f, (graphicsAPI ? -1.0f : 1.0f), 0.0f);

    //    transform.m_inverse = Matrix4x4(
    //        0.5f * far * (right - left), 0.0f, 0.0f, 0.5f * far * (left +
    //        right), 0.0f, (flipY ? -1.0f : 1.0f) * 0.5f * far * (top -
    //        bottom), 0.0f, 0.5f * far * (bottom + top), 0.0f, 0.0f, 0.0f, far
    //        * near, 0.0f, 0.0f, 0.5f * (far - near), 0.5f * (-far - near) +
    //        far + near);
    return transform;
}