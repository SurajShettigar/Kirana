#include "quaternion.hpp"

#include "math_utils.hpp"
#include "vector3.hpp"

const kirana::math::Quaternion kirana::math::Quaternion::IDENTITY;

kirana::math::Quaternion::Quaternion(const Quaternion &q)
{
    if (this != &q)
    {
        m_w = q.m_w;
        m_v = q.m_v;
    }
}
kirana::math::Quaternion &kirana::math::Quaternion::operator=(
    const Quaternion &q)
{
    if (this != &q)
    {
        m_w = q.m_w;
        m_v = q.m_v;
    }
    return *this;
}

bool kirana::math::Quaternion::operator==(const Quaternion &rhs) const
{
    return approximatelyEqual(m_w, rhs.m_w) && m_v == rhs.m_v;
}

bool kirana::math::Quaternion::operator!=(const Quaternion &rhs) const
{
    return !approximatelyEqual(m_w, rhs.m_w) || m_v != rhs.m_v;
}

kirana::math::Vector3 kirana::math::operator*(const Quaternion &quaternion,
                                              const Vector3 &vector)
{
    return (quaternion * Quaternion(vector, 0.0f) *
            Quaternion::inverse(quaternion))
        .m_v;
}

kirana::math::Quaternion kirana::math::operator*(const Quaternion &lhs,
                                                 const Quaternion &rhs)
{
    return Quaternion(
        Vector3{
            lhs.m_w * rhs.m_v[0] + lhs.m_v[0] * rhs.m_w +
                lhs.m_v[1] * rhs.m_v[2] - lhs.m_v[2] * rhs.m_v[1],
            lhs.m_w * rhs.m_v[1] + lhs.m_v[1] * rhs.m_w +
                lhs.m_v[2] * rhs.m_v[0] - lhs.m_v[0] * rhs.m_v[2],
            lhs.m_w * rhs.m_v[2] + lhs.m_v[2] * rhs.m_w +
                lhs.m_v[0] * rhs.m_v[1] - lhs.m_v[1] * rhs.m_v[0],
        },
        lhs.m_w * rhs.m_w - lhs.m_v[0] * rhs.m_v[0] - lhs.m_v[1] * rhs.m_v[1] -
            lhs.m_v[2] * rhs.m_v[2]);
}

kirana::math::Quaternion &kirana::math::Quaternion::operator*=(
    const Quaternion &rhs)
{
    return (*this = *this * rhs);
}

kirana::math::Vector3 kirana::math::Quaternion::rotateVector(
    const Vector3 &vector) const
{
    return (*this * vector);
}

kirana::math::Quaternion kirana::math::Quaternion::angleAxis(
    float angle, const Vector3 &axis)
{
    return Quaternion(axis * std::sinf(radians(angle * 0.5f)),
                      std::cosf(radians(angle * 0.5f)));
}

kirana::math::Quaternion kirana::math::Quaternion::inverse(
    const Quaternion &quaternion)
{
    return Quaternion(-quaternion.m_v, quaternion.m_w);
}

kirana::math::Quaternion kirana::math::Quaternion::difference(
    const Quaternion &from, const Quaternion &to)
{
    return to * Quaternion::inverse(from);
}

kirana::math::Quaternion kirana::math::Quaternion::exponent(
    const Quaternion &quaternion, float exponent)
{
    if (!approximatelyEqual(std::fabsf(quaternion.m_w), 1.0f))
    {
        Quaternion q;
        float alpha = std::acos(quaternion.m_w);
        float newAlpha = alpha * exponent;
        q.m_w = std::cos(newAlpha);
        q.m_v = quaternion.m_v * std::sin(newAlpha) / std::cos(alpha);
        return q;
    }
    return Quaternion::IDENTITY;
}