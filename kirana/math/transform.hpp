#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "matrix4x4.hpp"
#include "vector3.hpp"
#include <string>

namespace kirana::math
{

class Transform
{
    enum class Space
    {
        Local = 0,
        World = 1
    };

  private:
    Transform *m_parent = nullptr;
    Matrix4x4 m_localMatrix;
    Vector3 m_localPosition;
    Vector3 m_localRotation;
    Vector3 m_localScale;

    void calculateLocalMatrix();
    [[nodiscard]] Matrix4x4 getParentMatrix() const;
    [[nodiscard]] Matrix4x4 getGlobalMatrix() const;
  public:
    explicit Transform(Transform *parent = nullptr);
    explicit Transform(const Matrix4x4 &mat, Transform *parent = nullptr);
    ~Transform() = default;

    Transform(const Transform &transform);
    Transform &operator=(const Transform &transform);

    bool operator==(const Transform &rhs) const;
    bool operator!=(const Transform &rhs) const;

    [[nodiscard]] inline Transform *getParent() const
    {
        return m_parent;
    }
    inline void setParent(Transform *transform)
    {
        m_parent = transform;
    }

    [[nodiscard]] Matrix4x4 getMatrix(Space space = Space::World) const;

    [[nodiscard]] Vector3 getRight(Space space = Space::World) const;
    [[nodiscard]] Vector3 getUp(Space space = Space::World) const;
    [[nodiscard]] Vector3 getForward(Space space = Space::World) const;

    [[nodiscard]] Vector3 getPosition(Space space = Space::World) const;
    [[nodiscard]] Vector3 getRotation(Space space = Space::World) const;
    [[nodiscard]] Vector3 getScale(Space space = Space::Local) const;

    void setPosition(const Vector3 &position, Space space = Space::World);
    void setRotation(const Vector3 &rotation, Space space = Space::World);
    void setLocalScale(const Vector3 &scale);

    void translate(const Vector3 &translation, Space space = Space::World);
    void rotateX(float angle, Space space = Space::World);
    void rotateY(float angle, Space space = Space::World);
    void rotateZ(float angle, Space space = Space::World);
    void rotate(const Vector3 &rotation, Space space = Space::World);
    void rotateAround(float angle, const Vector3 &axis, Space space = Space::World);
    void lookAt(const Vector3 &lookAtPos, const Vector3 &up, Space space = Space::World);

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
};
} // namespace kirana::math
#endif