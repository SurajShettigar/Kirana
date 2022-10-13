#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "matrix4x4.hpp"

namespace kirana::math
{
class Vector3;
class Transform
{
  private:
    Transform *m_parent = nullptr;
    bool m_isDirty = false;
    mutable Matrix4x4 m_current;
    mutable Matrix4x4 m_inverse;

  public:
    explicit Transform(Matrix4x4 mat = Matrix4x4::IDENTITY,
                       Matrix4x4 matInverse = Matrix4x4::IDENTITY,
                       Transform *parent = nullptr);
    ~Transform() = default;

    Transform(const Transform &transform);
    Transform &operator=(const Transform &transform);

    bool operator==(const Transform &rhs) const;
    bool operator!=(const Transform &rhs) const;

    Transform &operator*=(const Transform &rhs);

    [[nodiscard]] inline Transform *getParent() const
    {
        return m_parent;
    }
    inline void setParent(Transform *transform)
    {
        m_parent = transform;
    }
    [[nodiscard]] inline bool isDirty() const
    {
        return m_isDirty;
    }
    inline void setDirty(bool value)
    {
        m_isDirty = value;
    }
    [[nodiscard]] const Matrix4x4 &getMatrix(bool inverse = false) const;
    [[nodiscard]] Matrix4x4 getMatrixTransposed(bool inverse = false) const;

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