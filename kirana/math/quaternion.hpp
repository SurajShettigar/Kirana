#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "matrix4x4.hpp"
#include "vector3.hpp"

namespace kirana::math
{
/**
 * Represents a unit length quaternion rotation.
 */
class Quaternion
{
  private:
    float m_w;
    Vector3 m_v;

  public:
    static const Quaternion IDENTITY;

    Quaternion() : m_w{1.0f}, m_v{0.0f, 0.0f, 0.0f} {};
    ~Quaternion() = default;
    explicit Quaternion(float x, float y, float z, float w)
        : m_w{w}, m_v{x, y, z} {};
    explicit Quaternion(const Vector3 &v, float w) : m_w{w}, m_v{v} {};
    explicit Quaternion(const Vector4 &vector)
        : m_w{vector[3]}, m_v{vector[0], vector[1], vector[2]} {};

    Quaternion(const Quaternion &q);
    Quaternion &operator=(const Quaternion &q);

    bool operator==(const Quaternion &rhs) const;
    bool operator!=(const Quaternion &rhs) const;


    friend Vector3 operator*(const Quaternion &quaternion,
                             const Vector3 &vector);
    friend Quaternion operator*(const Quaternion &lhs, const Quaternion &rhs);
    Quaternion &operator*=(const Quaternion &rhs);

    /// Rotates the given vector by the current rotation.
    [[nodiscard]] Vector3 rotateVector(const Vector3 &vector) const;
    /// Returns rotation as a rotation matrix.
    [[nodiscard]] Matrix4x4 getMatrix() const;
    /// Returns rotation as euler angles in degrees.
    [[nodiscard]] Vector3 getEulerAngles() const;

    // Quaternion construction functions
    static Quaternion matrix(const Matrix4x4 &mat);
    /**
     * Construct a quaternion from the euler angles.
     * @param euler The Euler angles in degrees.
     * @return Rotation in Quaternion form.
     */
    static Quaternion euler(const Vector3 &euler);
    /**
     * Construct a quaternion from the angle-axis rotation.
     * @param angle The rotation angle in degrees.
     * @param axis The normalized rotation axis.
     * @return Rotation in Quaternion form.
     */
    static Quaternion angleAxis(float angle, const Vector3 &axis);

    // Quaternion functions
    /// Get the inverse of a quaternion (conjugate).
    static Quaternion inverse(const Quaternion &quaternion);
    /// Get the difference between rotations.
    static Quaternion difference(const Quaternion &from, const Quaternion &to);
    /**
     * Get the fraction or multiple of a rotation
     * @param quaternion The rotation value.
     * @param exponent Fraction or multiple of the rotation. For example, if the
     * value is 0.5, a rotation half of the original value will be returned.
     * @return The fractional or multiplied rotation.
     */
    static Quaternion exponent(const Quaternion &quaternion, float exponent);
    /**
     * Spherical linear interpolation between two rotations.
     * @param from The rotation to interpolate from.
     * @param to The rotation to interpolate to
     * @param t The interpolation parameter with range [0, 1]
     * @return Interpolated rotation.
     */
    static Quaternion slerp(const Quaternion &from, const Quaternion &to,
                            float t);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Quaternion &q);
};
} // namespace kirana::math
#endif