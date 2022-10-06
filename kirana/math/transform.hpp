#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "matrix4x4.hpp"

namespace kirana::math
{
class Vector3;
class Transform
{
  private:
    Matrix4x4 m_current;
    Matrix4x4 m_inverse;

  public:
    Transform();
    explicit Transform(const Matrix4x4 &mat);
    explicit Transform(const Matrix4x4 &mat, const Matrix4x4 &matInverse);
    ~Transform() = default;

    Transform(const Transform &transform);
    Transform &operator=(const Transform &transform);

    bool operator==(const Transform &rhs) const;
    bool operator!=(const Transform &rhs) const;

    Transform &operator*=(const Transform &rhs);

    void translate(const Vector3 &translation);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void rotate(const Vector3 &rotation);
    void rotateAround(float angle, const Vector3 &axis);
    void lookAt(const Vector3 &lookAtPos, const Vector3 &up);
    void scale(const Vector3 &scale);

    [[nodiscard]] inline size_t size() const
    {
        return sizeof(m_current);
    }

    [[nodiscard]] inline size_t count() const
    {
        return sizeof(m_current) / sizeof(float);
    }

    [[nodiscard]] inline const float *data() const
    {
        return &m_current.m_current[0][0];
    }

    static Transform inverse(const Transform &transform);
    static Transform transpose(const Transform &transform);
    static Transform getOrthographicTransform(float left, float right,
                                              float bottom, float top,
                                              float near, float far, bool flipY = false);
    static Transform getOrthographicTransform(float size, float aspectRatio,
                                              float near, float far, bool flipY = false);
    static Transform getPerspectiveTransform(float fov, float aspectRatio,
                                             float near, float far, bool flipY = false);

    friend Transform operator*(const Transform &lhs, const Transform &rhs);
    friend Vector4 operator*(const Transform &lhs, const Vector4 &rhs);

    const Matrix4x4 &matrix = m_current;
    const Matrix4x4 &inverseMatrix = m_inverse;
};
} // namespace kirana::math
#endif