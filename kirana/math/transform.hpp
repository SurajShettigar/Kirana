#ifndef KIRANA_MATH_TRANSFORM_HPP
#define KIRANA_MATH_TRANSFORM_HPP

#include "quaternion.hpp"
#include "bounds3.hpp"
#include "ray.hpp"

namespace kirana::math
{

class Transform
{
  public:
    Transform() = default;
    explicit Transform(const Matrix4x4 &mat) : m_matrix{mat}
    {
        calculateComponents();
    }
    ~Transform() = default;

    Transform(const Transform &transform)
    {
        if (this != &transform)
        {
            m_matrix = transform.m_matrix;
            m_position = transform.m_position;
            m_rotation = transform.m_rotation;
            m_scale = transform.m_scale;
        }
    }
    Transform &operator=(const Transform &transform)
    {
        if (this != &transform)
        {
            m_matrix = transform.m_matrix;
            m_position = transform.m_position;
            m_rotation = transform.m_rotation;
            m_scale = transform.m_scale;
        }
        return *this;
    }

    bool operator==(const Transform &rhs) const
    {
        return m_matrix == rhs.m_matrix;
    }
    bool operator!=(const Transform &rhs) const
    {
        return !(*this == rhs);
    }

    explicit operator std::string() const
    {
        return "{Position: " + static_cast<std::string>(m_position) + ", " +
               "Rotation: " + static_cast<std::string>(m_rotation) + ", " +
               "Scale: " + static_cast<std::string>(m_scale) + "}";
    }

    Transform &operator*=(const Transform &rhs)
    {
        m_matrix *= rhs.m_matrix;
        calculateComponents();
    }

    friend Transform operator*(const Transform &lhs, const Transform &rhs)
    {
        return Transform(lhs.m_matrix * rhs.m_matrix);
    }
    friend Vector4 operator*(const Transform &lhs, const Vector4 &rhs)
    {
        return lhs.m_matrix * rhs;
    }
    friend Vector3 operator*(const Transform &lhs, const Vector3 &rhs)
    {
        return lhs.m_matrix * rhs;
    }
    friend std::ostream &operator<<(std::ostream &out, const Transform &transform)
    {
        return out << static_cast<std::string>(transform);
    }

    [[nodiscard]] inline size_t size() const
    {
        return m_matrix.size();
    }

    [[nodiscard]] inline const Vector4 *data() const
    {
        return m_matrix.data();
    }

    [[nodiscard]] inline const Matrix4x4 &getMatrix() const
    {
        return m_matrix;
    }
    inline Matrix4x4 getMatrix()
    {
        return m_matrix;
    }

    [[nodiscard]] inline Vector3 getRight() const
    {
        return Vector3::normalize(getRotation().rotateVector(Vector3::RIGHT));
    }
    [[nodiscard]] inline Vector3 getUp() const
    {
        return Vector3::normalize(getRotation().rotateVector(Vector3::UP));
    }
    [[nodiscard]] inline Vector3 getForward() const
    {
        return Vector3::normalize(getRotation().rotateVector(Vector3::FORWARD));
    }

    /**
     * Change the orientation of the transform by setting the forward direction.
     * @param forward The normalized forward direction vector.
     * @param space If Space::World, the forward direction vector is assumed to
     * be in world-space. If Space::Local, the vector is assumed to be in local
     * space.
     */
    void setForward(const Vector3 &forward)
    {
        const Vector3 right = Vector3::cross(Vector3::UP, forward);
        const Vector3 up = Vector3::cross(forward, right);
        m_matrix[0][0] = right[0];
        m_matrix[1][0] = right[1];
        m_matrix[2][0] = right[2];
        m_matrix[0][1] = up[0];
        m_matrix[1][1] = up[1];
        m_matrix[2][1] = up[2];
        m_matrix[0][2] = forward[0];
        m_matrix[1][2] = forward[1];
        m_matrix[2][2] = forward[2];

        Matrix4x4::decompose(m_matrix, nullptr, &m_rotation);
        calculateMatrix();
    }

    [[nodiscard]] inline const Vector3 &getPosition() const
    {
        return m_position;
    }
    [[nodiscard]] inline Vector3 getPosition()
    {
        return m_position;
    }
    [[nodiscard]] inline const Quaternion &getRotation() const
    {
        return m_rotation;
    }
    [[nodiscard]] inline Quaternion getRotation()
    {
        return m_rotation;
    }
    [[nodiscard]] inline const Vector3 &getScale() const
    {
        return m_scale;
    }
    [[nodiscard]] inline Vector3 getScale()
    {
        return m_scale;
    }

    inline void setPosition(const Vector3 &position)
    {
        m_position = position;
        calculateMatrix();
    }
    inline void setPositionInLocalAxis(const Vector3 &position)
    {
        m_position = getForward() * position[2] + getUp() * position[1] +
                     getRight() * position[0];
        calculateMatrix();
    }
    inline void setRotation(const Quaternion &rotation)
    {
        m_rotation = rotation;
        calculateMatrix();
    }
    inline void setRotation(const Vector3 &rotation)
    {
        setRotation(Quaternion::euler(rotation));
    }
    inline void setScale(const Vector3 &scale)
    {
        m_scale = scale;
        calculateMatrix();
    }

    /**
     * Transforms the given vector to the current transform space. The vector is
     *  affected by the scale of the transform. Position of the transform has no
     *  effect.
     * @param vector The vector to transform.
     * @return The transformed vector.
     */
    [[nodiscard]] inline Vector3 transformVector(const Vector3 &vector) const
    {
        return static_cast<Vector3>(getMatrix() * Vector4(vector, 0.0f));
    }
    /**
     * Transforms the given position (point) to the current transform space. The
     * vector is affected by the scale of the transform.
     * @param position The vector to transform.
     * @return The transformed position (point).
     */
    [[nodiscard]] inline Vector3 transformPosition(
        const Vector3 &position) const
    {
        return static_cast<Vector3>(getMatrix() * Vector4(position, 1.0f));
    }
    /**
     * Transforms the given direction to the current transform space. The
     * direction is not affected by the scale or the position of the transform.
     * @param direction The direction to transform.
     * @return The transformed direction.
     */
    [[nodiscard]] inline Vector3 transformDirection(
        const Vector3 &direction) const
    {
        return static_cast<Vector3>(Matrix4x4::translation(m_position) *
                                    m_rotation.getMatrix() *
                                    Vector4(direction, 0.0f));
    }

    /**
     * Transforms the given bounding-box to the current transform space.
     * @param bounds The bounding-box to transform.
     * @return The transformed bounding-box.
     */
    [[nodiscard]] Bounds3 transformBounds(const Bounds3 &bounds) const
    {
        // Algorithm taken from Graphic Gems: "Transforming Axis-Aligned
        // Bounding Boxes", by James Arvo.

        const Matrix4x4 &m = getMatrix();
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

    /**
     * Transforms the given ray to the current transform space.
     * @param ray The ray to transform.
     * @return The transformed ray.
     */
    [[nodiscard]] inline Ray transformRay(const Ray &ray) const
    {
        return {transformPosition(ray.m_origin),
                transformDirection(ray.m_direction), ray.m_tMax};
    }

    inline void translate(const Vector3 &translation)
    {
        m_position += translation;
        calculateMatrix();
    }
    inline void translateInLocalAxis(const Vector3 &translation)
    {
        m_position += getForward() * translation[2];
        m_position += getUp() * translation[1];
        m_position += getRight() * translation[0];
        calculateMatrix();
    }
    inline void rotateX(float angle)
    {
        m_rotation *= Quaternion::matrix(Matrix4x4::rotationX(angle));
        calculateMatrix();
    }
    inline void rotateY(float angle)
    {
        m_rotation *= Quaternion::matrix(Matrix4x4::rotationY(angle));
        calculateMatrix();
    }
    inline void rotateZ(float angle)
    {
        m_rotation *= Quaternion::matrix(Matrix4x4::rotationZ(angle));
        calculateMatrix();
    }
    inline void rotate(const Vector3 &rotation)
    {
        m_rotation *= Quaternion::euler(rotation);
        calculateMatrix();
    }
    inline void rotate(const Quaternion &rotation)
    {
        m_rotation *= rotation;
        calculateMatrix();
    }
    inline void rotateAround(float angle, const Vector3 &axis)
    {
        m_rotation *= Quaternion::angleAxis(angle, axis);
        calculateMatrix();
    }
    void lookAt(const Vector3 &position, const Vector3 &up)
    {
        const Vector3 eyePos = getPosition();
        const Vector3 z = -Vector3::normalize(position - eyePos);
        const Vector3 x =
            Vector3::normalize(Vector3::cross(Vector3::normalize(up), z));
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

  private:
    Matrix4x4 m_matrix;
    Vector3 m_position;
    Quaternion m_rotation;
    Vector3 m_scale;

    inline void calculateMatrix()
    {
        m_matrix = Matrix4x4::translation(m_position) * m_rotation.getMatrix() *
                   Matrix4x4::scale(m_scale);
    }

    inline void calculateComponents()
    {
        Vector3 eulerAngles;
        Matrix4x4::decompose(m_matrix, &m_position, &m_scale, nullptr,
                             &eulerAngles);
        m_rotation = Quaternion::euler(eulerAngles);
    }
};
} // namespace kirana::math
#endif