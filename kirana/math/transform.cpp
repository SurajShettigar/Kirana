#include "transform.hpp"

#include "vector3.hpp"
#include "vector4.hpp"


void kirana::math::Transform::calculateLocalMatrix()
{
    m_localMatrix = Matrix4x4::translation(m_localPosition) *
                    m_localRotation.getMatrix() *
                    Matrix4x4::scale(m_localScale);
}


kirana::math::Matrix4x4 kirana::math::Transform::getParentMatrix(
    bool includeScale) const
{
    if (m_parent != nullptr && m_parent != this)
        return m_parent->getWorldMatrix(includeScale);
    return Matrix4x4::IDENTITY;
}

kirana::math::Matrix4x4 kirana::math::Transform::getWorldMatrix(
    bool includeScale) const
{
    if (includeScale)
    {
        if (m_parent != nullptr && m_parent != this)
            return m_parent->getWorldMatrix(includeScale) * m_localMatrix;
        return m_localMatrix;
    }
    else
    {
        if (m_parent != nullptr && m_parent != this)
            return m_parent->getWorldMatrix(includeScale) *
                   Matrix4x4::translation(m_localPosition) *
                   m_localRotation.getMatrix();
        return Matrix4x4::translation(m_localPosition) *
               m_localRotation.getMatrix();
    }
}

kirana::math::Transform::Transform(Transform *parent)
    : m_parent{parent}, m_localMatrix{Matrix4x4::IDENTITY}, m_localPosition{},
      m_localRotation{}, m_localScale{Vector3::ONE}
{
}

kirana::math::Transform::Transform(const Matrix4x4 &mat, Transform *parent)
    : m_parent{parent}, m_localMatrix{mat}
{
    Vector3 eulerAngles;
    Matrix4x4::decompose(m_localMatrix, &m_localPosition, &m_localScale,
                         nullptr, &eulerAngles);
    m_localRotation = Quaternion::euler(eulerAngles);
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
        return getWorldMatrix();
    return m_localMatrix;
}

kirana::math::Vector3 kirana::math::Transform::getRight(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getWorldMatrix();
        return Vector3(globalMat[0][0], globalMat[1][0], globalMat[2][0]);
    }
    return Vector3(m_localMatrix[0][0], m_localMatrix[1][0],
                   m_localMatrix[2][0]);
}

kirana::math::Vector3 kirana::math::Transform::getUp(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getWorldMatrix();
        return Vector3(globalMat[0][1], globalMat[1][1], globalMat[2][1]);
    }
    return Vector3(m_localMatrix[0][1], m_localMatrix[1][1],
                   m_localMatrix[2][1]);
}

kirana::math::Vector3 kirana::math::Transform::getForward(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getWorldMatrix();
        return Vector3(globalMat[0][2], globalMat[1][2], globalMat[2][2]);
    }
    return Vector3(m_localMatrix[0][2], m_localMatrix[1][2],
                   m_localMatrix[2][2]);
}

kirana::math::Vector3 kirana::math::Transform::getPosition(Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getWorldMatrix();
        return Vector3(globalMat[0][3], globalMat[1][3], globalMat[2][3]);
    }
    return m_localPosition;
}

kirana::math::Quaternion kirana::math::Transform::getRotation(Space space) const
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            Quaternion rot;
            Matrix4x4::decompose(getWorldMatrix(false), nullptr, &rot);
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
            Matrix4x4::decompose(getWorldMatrix(), nullptr, &scale, nullptr,
                                 nullptr);
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
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::euler(rotation);
        }
        else
            m_localRotation = Quaternion::euler(rotation);
    }
    else
        m_localRotation = Quaternion::euler(rotation);

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
            m_localPosition = static_cast<Vector3>(
                Matrix4x4::inverse(getParentMatrix()) *
                (Vector4(translation, 1.0f) +
                 getParentMatrix() * Vector4(m_localPosition, 1.0f)));
        }
        else
            m_localPosition += translation;
    }
    else
        m_localPosition += translation;

    calculateLocalMatrix();
}

void kirana::math::Transform::rotateX(float angle, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::matrix(Matrix4x4::rotationX(angle)) *
                Quaternion::matrix(getParentMatrix()) * m_localRotation;
        }
        else
            m_localRotation = Quaternion::matrix(Matrix4x4::rotationX(angle)) *
                              m_localRotation;
    }
    else
        m_localRotation =
            Quaternion::matrix(Matrix4x4::rotationX(angle)) * m_localRotation;

    calculateLocalMatrix();
}

void kirana::math::Transform::rotateY(float angle, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::matrix(Matrix4x4::rotationY(angle)) *
                Quaternion::matrix(getParentMatrix()) * m_localRotation;
        }
        else
            m_localRotation = Quaternion::matrix(Matrix4x4::rotationY(angle)) *
                              m_localRotation;
    }
    else
        m_localRotation =
            Quaternion::matrix(Matrix4x4::rotationY(angle)) * m_localRotation;

    calculateLocalMatrix();
}

void kirana::math::Transform::rotateZ(float angle, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::matrix(Matrix4x4::rotationZ(angle)) *
                Quaternion::matrix(getParentMatrix()) * m_localRotation;
        }
        else
            m_localRotation = Quaternion::matrix(Matrix4x4::rotationZ(angle)) *
                              m_localRotation;
    }
    else
        m_localRotation =
            Quaternion::matrix(Matrix4x4::rotationZ(angle)) * m_localRotation;

    calculateLocalMatrix();
}

void kirana::math::Transform::rotate(const Vector3 &rotation, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::euler(rotation) *
                Quaternion::matrix(getParentMatrix()) * m_localRotation;
        }
        else
            m_localRotation = Quaternion::euler(rotation) * m_localRotation;
    }
    else
        m_localRotation = Quaternion::euler(rotation) * m_localRotation;

    calculateLocalMatrix();
}

void kirana::math::Transform::rotateAround(float angle, const Vector3 &axis,
                                           Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                Quaternion::angleAxis(angle, axis) *
                Quaternion::matrix(getParentMatrix()) * m_localRotation;
        }
        else
            m_localRotation =
                Quaternion::angleAxis(angle, axis) * m_localRotation;
    }
    else
        m_localRotation = Quaternion::angleAxis(angle, axis) * m_localRotation;

    calculateLocalMatrix();
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

    Matrix4x4::decompose(m_localMatrix, &m_localPosition, &m_localRotation);
    calculateLocalMatrix();
}