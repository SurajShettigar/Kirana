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

    const Matrix4x4 &getMatrix(bool inverse = false) const;
    Matrix4x4 getMatrixTransposed(bool inverse = false) const;

    void translate(const Vector3 &translation);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void rotate(const Vector3 &rotation);
    void rotateAround(float angle, const Vector3 &axis);
    void lookAt(const Vector3 &lookAtPos, const Vector3 &up);
    void scale(const Vector3 &scale);

    static Transform inverse(const Transform &transform);
    static Transform transpose(const Transform &transform);
    static Transform getOrthographicTransform(float left, float right,
                                              float bottom, float top,
                                              float near, float far,
                                              bool graphicsAPI = false,
                                              bool flipY = false);
    static Transform getOrthographicTransform(float size, float aspectRatio,
                                              float near, float far,
                                              bool graphicsAPI = false,
                                              bool flipY = false);
    static Transform getPerspectiveTransform(float fov, float aspectRatio,
                                             float near, float far,
                                             bool graphicsAPI = false,
                                             bool flipY = false);

    friend Transform operator*(const Transform &lhs, const Transform &rhs);
    friend Vector4 operator*(const Transform &lhs, const Vector4 &rhs);
};
} // namespace kirana::math
#endif