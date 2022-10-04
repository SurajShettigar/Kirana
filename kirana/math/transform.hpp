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

    Transform(const Transform& transform);
    Transform &operator=(const Transform &transform);

    bool operator==(const Transform &rhs) const;
    bool operator!=(const Transform &rhs) const;

    void translate(const Vector3& translation);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void rotate(const Vector3& rotation);
    void rotateAround(float angle, const Vector3 &axis);
    void lookAt(const Vector3& lookAtPos, const Vector3& up);
    void scale(const Vector3& scale);

    static Transform inverse(const Transform& transform);
    static Transform transpose(const Transform& transform);

};
} // namespace kirana::math
#endif