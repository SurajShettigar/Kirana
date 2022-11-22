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

kirana::math::Matrix4x4 kirana::math::Quaternion::getMatrix() const
{
    const float &x = m_v[0];
    const float &y = m_v[1];
    const float &z = m_v[2];
    const float &w = m_w;

    return Matrix4x4(
        1.0f - 2.0f * y * y - 2.0f * z * z, 2.0f * x * y - 2.0f * w * z,
        2.0f * x * z + 2.0f * w * y, 0.0f, 2.0f * x * y + 2.0f * w * z,
        1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z - 2.0f * w * x, 0.0f,
        2.0f * x * z - 2 * w * y, 2.0f * y * z + 2.0f * w * x,
        1.0f - 2.0f * x * x - 2.0f * y * y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

kirana::math::Vector3 kirana::math::Quaternion::getEulerAngles() const
{
    // Taken from 3D Math Primer, by Fletcher Dunn and Ian Parbery
    // Refer: (Section 8.7.6)
    // https://gamemath.com/book/orient.html
    Vector3 euler;
    euler[0] = -2.0f * (m_v[1] * m_v[2] - m_w * m_v[0]);

    // If pitch = +-90 degrees, we need to set bank/roll to zero
    // to avoid Gimbal lock.
    if (fabsf(euler[0]) > 0.9999f)
    {
        // Set pitch to 90 degrees
        euler[0] = math::degrees(
            static_cast<float>(math::PI * 0.5 * static_cast<double>(euler[0])));
        // Heading/Yaw
        euler[1] = math::degrees(
            std::atan2f(-m_v[0] * m_v[2] + m_w * m_v[1],
                        0.5f - m_v[1] * m_v[1] * m_v[2] * m_v[2]));
        // Set Bank/Roll to 0
        euler[2] = 0.0f;
    }
    else
    {
        euler[0] = math::degrees(std::asinf(euler[0]));
        euler[1] = math::degrees(
            std::atan2f(m_v[0] * m_v[2] + m_w * m_v[1],
                        0.5f - m_v[0] * m_v[0] - m_v[1] * m_v[1]));
        euler[2] = math::degrees(
            std::atan2f(m_v[0] * m_v[1] + m_w * m_v[2],
                        0.5f - m_v[0] * m_v[0] - m_v[2] * m_v[2]));
    }

    return euler;
}

kirana::math::Quaternion kirana::math::Quaternion::matrix(const Matrix4x4 &mat)
{
    // TODO: Implement Quaternion construction from rotation matrix.
    return Quaternion::IDENTITY;
}

kirana::math::Quaternion kirana::math::Quaternion::euler(const Vector3 &euler)
{
    // Taken from 3D Math Primer, by Fletcher Dunn and Ian Parbery
    // Refer: (Section 8.7.5)
    // https://gamemath.com/book/orient.html

    // Pitch X-axis
    float cosP = std::cosf(math::radians(euler[0] * 0.5f));
    float sinP = std::sinf(math::radians(euler[0] * 0.5f));
    // Yaw / Heading Y-axis
    float cosH = std::cosf(math::radians(euler[1] * 0.5f));
    float sinH = std::sinf(math::radians(euler[1] * 0.5f));
    // Roll / Bank Z-axis
    float cosB = std::cosf(math::radians(euler[2] * 0.5f));
    float sinB = std::sinf(math::radians(euler[2] * 0.5f));

    // Perform Bank*Pitch*Heading fixed-axis rotation.
    return Quaternion(Vector3{cosH * sinP * cosB + sinH * cosP * sinB,
                              sinH * cosP * cosB - cosH * sinP * sinB,
                              cosH * cosP * sinB - sinH * sinP * cosB},
                      cosH * cosP * cosB + sinH * sinP * sinB);
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
    // Taken from 3D Math Primer, by Fletcher Dunn and Ian Parbery
    // Refer: (Section 8.5.11)
    // https://gamemath.com/book/orient.html
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


kirana::math::Quaternion kirana::math::Quaternion::slerp(const Quaternion &from,
                                                         const Quaternion &to,
                                                         float t)
{
    // Taken from 3D Math Primer, by Fletcher Dunn and Ian Parbery
    // Refer: (Section 8.5.12)
    // https://gamemath.com/book/orient.html

    t = math::clampf(t, 0.0f, 1.0f);
    float cosOmega = from.m_w * to.m_w + Vector3::dot(from.m_v, to.m_v);
    float k0 = 1.0f - t;
    float k1 = t;
    Quaternion newTo(to);

    // If cosOmega is negative, negate one of the quaternions to take the
    // shorter arc when interpolating.
    if (cosOmega < 0.0f)
    {
        newTo.m_w = -to.m_w;
        newTo.m_v = -to.m_v;
        cosOmega = -cosOmega;
    }

    // If two quaternions are different enough, we can calculate k0 and k1
    // interpolation parameters. Otherwise, use the simple linear interpolation
    // parameters as initialized above.
    if (cosOmega <= 0.9999f)
    {
        float sinOmega = std::sqrt(1.0f - cosOmega * cosOmega);
        float omega = std::atan2(sinOmega, cosOmega);
        float oneOverSinOmega = 1.0f / sinOmega;
        k0 = std::sinf((1.0f - t) * omega) * oneOverSinOmega;
        k1 = std::sinf(t * omega) * oneOverSinOmega;
    }

    return Quaternion(from.m_v * k0 + newTo.m_v * k1,
                      from.m_w * k0 + newTo.m_w * k1);
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Quaternion &q)
{
    return out << q.getEulerAngles();
}
