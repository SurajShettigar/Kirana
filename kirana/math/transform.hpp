#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "quaternion.hpp"
#include "bounds3.hpp"
#include "ray.hpp"

#include <event.hpp>
#include <string>

namespace kirana::math
{

class Transform
{
  public:
    enum class Space
    {
        Local = 0,
        World = 1
    };

  private:
    Transform *m_parent = nullptr;
    Matrix4x4 m_localMatrix;
    Vector3 m_localPosition;
    Quaternion m_localRotation;
    Vector3 m_localScale;

    bool m_enableEvents = false;
    utils::Event<> m_onChangeEvent;

    void calculateLocalMatrix();
    /**
     * Returns the world matrix of only the parent. Does not include the local
     * matrix.
     * @param includeScale if set to false, the matrix will only include
     * translation and rotation.
     * @return Matrix4x4
     */
    [[nodiscard]] Matrix4x4 getParentMatrix(bool includeScale = true) const;
    /**
     * Returns the world matrix including the current local matrix.
     * @param includeScale if set to false, the matrix will only include
     * translation and rotation.
     * @return Matrix4x4
     */
    [[nodiscard]] Matrix4x4 getWorldMatrix(bool includeScale = true) const;

  public:
    explicit Transform(Transform *parent = nullptr, bool enableEvents = false);
    explicit Transform(const Matrix4x4 &mat, Transform *parent = nullptr,
                       bool enableEvents = false);
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
        if (m_enableEvents)
            m_onChangeEvent();
    }

    inline uint32_t addOnChangeListener(const std::function<void()> &callback)
    {
        return m_onChangeEvent.addListener(callback);
    }

    inline void removeOnChangeListener(uint32_t callbackId)
    {
        m_onChangeEvent.removeListener(callbackId);
    }

    [[nodiscard]] Matrix4x4 getMatrix(Space space = Space::World) const;

    [[nodiscard]] Vector3 getRight(Space space = Space::World) const;
    [[nodiscard]] Vector3 getUp(Space space = Space::World) const;
    [[nodiscard]] Vector3 getForward(Space space = Space::World) const;

    /**
     * Change the orientation of the transform by setting the forward direction.
     * @param forward The normalized forward direction vector.
     * @param space If Space::World, the forward direction vector is assumed to
     * be in world-space. If Space::Local, the vector is assumed to be in local
     * space.
     */
    void setForward(const Vector3 &forward, Space space = Space::World);

    [[nodiscard]] Vector3 getPosition(Space space = Space::World) const;
    [[nodiscard]] Quaternion getRotation(Space space = Space::World) const;
    [[nodiscard]] Vector3 getScale(Space space = Space::Local) const;

    void setPosition(const Vector3 &position, Space space = Space::World);
    void setPositionInLocalAxis(const Vector3 &position);
    void setRotation(const Vector3 &rotation, Space space = Space::World);
    void setRotation(const Quaternion &rotation, Space space = Space::World);
    void setLocalScale(const Vector3 &scale);

    /**
     * Transforms the given vector to world/local space. The vector is affected
     * by the scale of the transform. Position of the transform has no effect.
     * @param vector The vector to transform.
     * @param space If Space::World, the given local vector is transformed into
     * a world space vector. If Space::Local, the given world vector is
     * transformed into a local vector.
     * @return The transformed vector.
     */
    [[nodiscard]] Vector3 transformVector(const Vector3 &vector,
                                          Space space = Space::World) const;
    /**
     * Transforms the given position (point) to world/local space. The vector is
     * affected by the scale of the transform.
     * @param position The vector to transform.
     * @param space If Space::World, the given local position is transformed
     * into a world space position. If Space::Local, the given world position is
     * transformed into a local position.
     * @return The transformed position (point).
     */
    [[nodiscard]] Vector3 transformPosition(const Vector3 &position,
                                            Space space = Space::World) const;
    /**
     * Transforms the given direction to world/local space. The direction is
     * not affected by the scale or the position of the transform.
     * @param direction The direction to transform.
     * @param space If Space::World, the given local direction is transformed
     * into a world space direction. If Space::Local, the given world direction
     * is transformed into a local direction.
     * @return The transformed direction.
     */
    [[nodiscard]] Vector3 transformDirection(const Vector3 &direction,
                                             Space space = Space::World) const;

    /**
     * Transforms the given bounding-box to world/local space.
     * @param bounds The bounding-box to transform.
     * @param space If Space::World, the given local-space bounding-box is
     * transformed into a world space bounding-box. If Space::Local, the given
     * world-space bounding-box is transformed into a local-space bounding-box.
     * @return The transformed bounding-box.
     */
    [[nodiscard]] Bounds3 transformBounds(const Bounds3 &bounds,
                                          Space space = Space::World) const;
    /**
     * Transforms the given ray to world/local space.
     * @param ray The ray to transform.
     * @param space If Space::World, the given local-space ray is transformed
     * into a world space ray. If Space::Local, the given world-space ray is
     * transformed into a local-space ray.
     * @return The transformed ray.
     */
    [[nodiscard]] inline Ray transformRay(const Ray &ray,
                                          Space space = Space::World) const
    {
        return {transformPosition(ray.m_origin, space),
                transformDirection(ray.m_direction, space), ray.m_tMax};
    }

    void translate(const Vector3 &translation, Space space = Space::World);
    void translateInLocalAxis(const Vector3 &translation);
    void rotateX(float angle, Space space = Space::World);
    void rotateY(float angle, Space space = Space::World);
    void rotateZ(float angle, Space space = Space::World);
    void rotate(const Vector3 &rotation, Space space = Space::World);
    void rotate(const Quaternion &rotation, Space space = Space::World);
    void rotateAround(float angle, const Vector3 &axis,
                      Space space = Space::World);
    void lookAt(const Vector3 &direction, const Vector3 &up);
};
} // namespace kirana::math
#endif