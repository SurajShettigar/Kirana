#include "transform_hierarchy.hpp"

#include "vector3.hpp"
#include "vector4.hpp"


void kirana::math::TransformHierarchy::calculateLocalMatrix()
{
    m_localMatrix = Matrix4x4::translation(m_localPosition) *
                    m_localRotation.getMatrix() *
                    Matrix4x4::scale(m_localScale);
}


kirana::math::Matrix4x4 kirana::math::TransformHierarchy::getParentMatrix(
    bool includeScale) const
{
    if (m_parent != nullptr && m_parent != this)
        return m_parent->getWorldMatrix(includeScale);
    return Matrix4x4::IDENTITY;
}

kirana::math::Matrix4x4 kirana::math::TransformHierarchy::getWorldMatrix(
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

kirana::math::TransformHierarchy::TransformHierarchy(TransformHierarchy *parent)
    : m_parent{parent}, m_localMatrix{Matrix4x4::IDENTITY}, m_localPosition{},
      m_localRotation{}, m_localScale{Vector3::ONE}
{
}

kirana::math::TransformHierarchy::TransformHierarchy(const Matrix4x4 &mat,
                                                     TransformHierarchy *parent)
    : m_parent{parent}, m_localMatrix{mat}
{
    Vector3 eulerAngles;
    Matrix4x4::decompose(m_localMatrix, &m_localPosition, &m_localScale,
                         nullptr, &eulerAngles);
    m_localRotation = Quaternion::euler(eulerAngles);
}


kirana::math::TransformHierarchy::TransformHierarchy(
    const TransformHierarchy &TransformHierarchy)
{
    if (this != &TransformHierarchy)
    {
        m_parent = TransformHierarchy.m_parent;
        m_localMatrix = TransformHierarchy.m_localMatrix;
        m_localPosition = TransformHierarchy.m_localPosition;
        m_localRotation = TransformHierarchy.m_localRotation;
        m_localScale = TransformHierarchy.m_localScale;
    }
}

kirana::math::TransformHierarchy &kirana::math::TransformHierarchy::operator=(
    const TransformHierarchy &TransformHierarchy)
{
    if (this != &TransformHierarchy)
    {
        m_parent = TransformHierarchy.m_parent;
        m_localMatrix = TransformHierarchy.m_localMatrix;
        m_localPosition = TransformHierarchy.m_localPosition;
        m_localRotation = TransformHierarchy.m_localRotation;
        m_localScale = TransformHierarchy.m_localScale;
    }
    return *this;
}

bool kirana::math::TransformHierarchy::operator==(
    const TransformHierarchy &rhs) const
{
    return m_localMatrix == rhs.m_localMatrix && m_parent == rhs.m_parent;
}

bool kirana::math::TransformHierarchy::operator!=(
    const TransformHierarchy &rhs) const
{
    return !(*this == rhs);
}

kirana::math::Matrix4x4 kirana::math::TransformHierarchy::getMatrix(
    Space space) const
{
    if (space == Space::World)
        return getWorldMatrix();
    return m_localMatrix;
}

kirana::math::Vector3 kirana::math::TransformHierarchy::getRight(
    Space space) const
{
    return Vector3::normalize(getRotation(space).rotateVector(Vector3::RIGHT));
}

kirana::math::Vector3 kirana::math::TransformHierarchy::getUp(Space space) const
{
    return Vector3::normalize(getRotation(space).rotateVector(Vector3::UP));
}

kirana::math::Vector3 kirana::math::TransformHierarchy::getForward(
    Space space) const
{
    return Vector3::normalize(
        getRotation(space).rotateVector(Vector3::FORWARD));
}


void kirana::math::TransformHierarchy::setForward(const Vector3 &forward,
                                                  Space space)
{
    Vector3 right;
    Vector3 up;
    if (space == Space::World)
    {
        const Vector3 fwd =
            transformDirection(Vector3::normalize(forward), Space::Local);
        right = Vector3::cross(Vector3::UP, fwd);
        up = Vector3::cross(fwd, right);
    }
    else
    {
        right = Vector3::cross(Vector3::UP, forward);
        up = Vector3::cross(forward, right);
    }
    m_localMatrix[0][0] = right[0];
    m_localMatrix[1][0] = right[1];
    m_localMatrix[2][0] = right[2];
    m_localMatrix[0][1] = up[0];
    m_localMatrix[1][1] = up[1];
    m_localMatrix[2][1] = up[2];
    m_localMatrix[0][2] = forward[0];
    m_localMatrix[1][2] = forward[1];
    m_localMatrix[2][2] = forward[2];

    Matrix4x4::decompose(m_localMatrix, nullptr, &m_localRotation);
    calculateLocalMatrix();
}

kirana::math::Vector3 kirana::math::TransformHierarchy::getPosition(
    Space space) const
{
    if (space == Space::World)
    {
        Matrix4x4 globalMat = getWorldMatrix();
        return Vector3(globalMat[0][3], globalMat[1][3], globalMat[2][3]);
    }
    return m_localPosition;
}

kirana::math::Quaternion kirana::math::TransformHierarchy::getRotation(
    Space space) const
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

kirana::math::Vector3 kirana::math::TransformHierarchy::getScale(
    Space space) const
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

void kirana::math::TransformHierarchy::setPosition(const Vector3 &position,
                                                   Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localPosition = Matrix4x4::inverse(getParentMatrix()) * position;
        }
        else
            m_localPosition = position;
    }
    else
    {
        m_localPosition = position;
    }
    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::setPositionInLocalAxis(
    const Vector3 &position)
{
    m_localPosition = getForward() * position[2];
    m_localPosition = getUp() * position[1];
    m_localPosition = getRight() * position[0];
    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::setRotation(const Vector3 &rotation,
                                                   Space space)
{
    setRotation(Quaternion::euler(rotation), space);
}

void kirana::math::TransformHierarchy::setRotation(const Quaternion &rotation,
                                                   Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                rotation;
        }
        else
            m_localRotation = rotation;
    }
    else
        m_localRotation = rotation;

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::setLocalScale(const Vector3 &scale)
{
    m_localScale = scale;
    calculateLocalMatrix();
}

kirana::math::Vector3 kirana::math::TransformHierarchy::transformVector(
    const Vector3 &vector, Space space) const
{
    if (space == Space::World)
        return static_cast<Vector3>(getWorldMatrix() * Vector4(vector, 0.0f));
    else
        return static_cast<Vector3>(Matrix4x4::inverse(getWorldMatrix()) *
                                    Vector4(vector, 0.0f));
}


kirana::math::Vector3 kirana::math::TransformHierarchy::transformPosition(
    const Vector3 &position, Space space) const
{
    if (space == Space::World)
        return static_cast<Vector3>(getWorldMatrix() * Vector4(position, 1.0f));
    else
        return static_cast<Vector3>(Matrix4x4::inverse(getWorldMatrix()) *
                                    Vector4(position, 1.0f));
}


kirana::math::Vector3 kirana::math::TransformHierarchy::transformDirection(
    const Vector3 &direction, Space space) const
{
    if (space == Space::World)
        return static_cast<Vector3>(getWorldMatrix(false) *
                                    Vector4(direction, 0.0f));
    else
        return static_cast<Vector3>(Matrix4x4::inverse(getWorldMatrix(false)) *
                                    Vector4(direction, 0.0f));
}

kirana::math::Bounds3 kirana::math::TransformHierarchy::transformBounds(
    const Bounds3 &bounds, Space space) const
{
    // Algorithm taken from Graphic Gems: "Transforming Axis-Aligned Bounding
    // Boxes", by James Arvo.

    const Matrix4x4 &m = space == Space::World
                             ? getWorldMatrix()
                             : Matrix4x4::inverse(getWorldMatrix());
    const Vector3 &translation = Vector3(m[0][3], m[1][3], m[2][3]);
    Bounds3 tBounds(translation, translation);

    float a = 0.0f, b = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        // x-axis
        a = m[0][i] * bounds.m_min[i];
        b = m[0][i] * bounds.m_max[i];

        tBounds.m_min[0] += std::fminf(a, b);
        tBounds.m_max[0] += std::fmaxf(a, b);

        // y-axis
        a = m[1][i] * bounds.m_min[i];
        b = m[1][i] * bounds.m_max[i];

        tBounds.m_min[1] += std::fminf(a, b);
        tBounds.m_max[1] += std::fmaxf(a, b);

        // z-axis
        a = m[2][i] * bounds.m_min[i];
        b = m[2][i] * bounds.m_max[i];

        tBounds.m_min[2] += std::fminf(a, b);
        tBounds.m_max[2] += std::fmaxf(a, b);
    }
    return tBounds;
}


void kirana::math::TransformHierarchy::translate(
    const kirana::math::Vector3 &translation, Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localPosition =
                Matrix4x4::inverse(getParentMatrix()) *
                (translation + getParentMatrix() * m_localPosition);
        }
        else
        {
            m_localPosition += translation;
        }
    }
    else
    {
        m_localPosition += translation;
    }

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::translateInLocalAxis(
    const Vector3 &translation)
{
    m_localPosition += getForward() * translation[2];
    m_localPosition += getUp() * translation[1];
    m_localPosition += getRight() * translation[0];
    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::rotateX(float angle, Space space)
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
        m_localRotation *= Quaternion::matrix(Matrix4x4::rotationX(angle));

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::rotateY(float angle, Space space)
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
        m_localRotation *= Quaternion::matrix(Matrix4x4::rotationY(angle));

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::rotateZ(float angle, Space space)
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
        m_localRotation *= Quaternion::matrix(Matrix4x4::rotationZ(angle));

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::rotate(const Vector3 &rotation,
                                              Space space)
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
        m_localRotation *= Quaternion::euler(rotation);

    calculateLocalMatrix();
}


void kirana::math::TransformHierarchy::rotate(const Quaternion &rotation,
                                              Space space)
{
    if (space == Space::World)
    {
        if (m_parent != nullptr && m_parent != this)
        {
            m_localRotation =
                Quaternion::matrix(Matrix4x4::inverse(getParentMatrix())) *
                rotation * Quaternion::matrix(getParentMatrix()) *
                m_localRotation;
        }
        else
            m_localRotation = rotation * m_localRotation;
    }
    else
        m_localRotation *= rotation;

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::rotateAround(float angle,
                                                    const Vector3 &axis,
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
        m_localRotation *= Quaternion::angleAxis(angle, axis);

    calculateLocalMatrix();
}

void kirana::math::TransformHierarchy::lookAt(const Vector3 &position,
                                              const Vector3 &up)
{
    const Vector3 eyePos = getPosition();
    const Vector3 z = -Vector3::normalize(position - eyePos);
    const Vector3 x = Vector3::normalize(Vector3::cross(Vector3::normalize(up), z));
    const Vector3 y = Vector3::normalize(Vector3::cross(z, x));

    const Matrix4x4 &worldMat =
        Matrix4x4(x[0], y[0], z[0], eyePos[0], x[1], y[1], z[1], eyePos[1],
                  x[2], y[2], z[2], eyePos[2], 0.0f, 0.0f, 0.0f, 1.0f);

    Vector3 worldPos;
    Quaternion worldRot;
    Matrix4x4::decompose(worldMat, &worldPos, &worldRot);

    setRotation(worldRot);
    setPosition(worldPos);
}