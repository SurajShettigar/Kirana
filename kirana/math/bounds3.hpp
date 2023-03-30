#ifndef BOUNDS_3_HPP
#define BOUNDS_3_HPP

#include "vector3.hpp"

namespace kirana::math
{
class Ray;
class Transform;
class TransformHierarchy;
class Bounds3
{
    friend class Transform;
    friend class TransformHierarchy;

  private:
    Vector3 m_min;
    Vector3 m_max;
  public:
    Bounds3();
    /**
     * Creates a bounding box enclosing the given point.
     * @param point The point to enclose.
     */
    explicit Bounds3(const Vector3 &point);
    explicit Bounds3(const Vector3 &min, const Vector3 &max);
    ~Bounds3() = default;

    Bounds3(const Bounds3 &bounds);
    Bounds3 &operator=(const Bounds3 &bounds);
    bool operator==(const Bounds3 &rhs) const;
    bool operator!=(const Bounds3 &rhs) const;

    Vector3 operator[](int i) const
    {
        return (i == 0) ? m_min : m_max;
    }

    Vector3 &operator[](int i)
    {
        return (i == 0) ? m_min : m_max;
    }

    explicit operator std::string() const;

    // Getters-Setters
    [[nodiscard]] inline Vector3 getMin() const
    {
        return m_min;
    }
    [[nodiscard]] inline Vector3 getMax() const
    {
        return m_max;
    }
    [[nodiscard]] inline Vector3 getCenter() const
    {
        return (m_min + m_max) * 0.5f;
    }
    [[nodiscard]] inline Vector3 getSize() const
    {
        return (m_max - m_min);
    }
    [[nodiscard]] inline Vector3 getExtent() const
    {
        return getSize() * 0.5f;
    }

    /**
     * Returns the position of the corner of bounding-box.
     * @param corner index of the corner.
     *  <pre>
     *  <br>  6________7
     *  <br> /        /
     *  <br>2________3
     *  <br>
     *  <br>  4________5
     *  <br> /        /
     *  <br>0________1</pre>
     * @return The vertex position of the given corner.
     */
    [[nodiscard]] inline Vector3 getCorner(uint16_t corner) const
    {
        // Taken from PBRT, by Matt Pharr
        // Refer: Section 2.6
        return Vector3((*this)[corner & 1][0], (*this)[(corner & 2) ? 1 : 0][1],
                       (*this)[(corner & 4) ? 1 : 0][2]);
    }

    void encapsulate(const Vector3 &point);
    void encapsulate(const Bounds3 &bounds);
    void expand(float delta);
    [[nodiscard]] bool contains(const Vector3 &point) const;
    bool intersectWithRay(const Ray &ray, Vector3 *enterPoint,
                          Vector3 *exitPoint) const;
    /**
     * Faster version of ray-bounding-box intersection.
     * @param ray
     * @return true if the entire ray is inside the bounding box.
     * i.e. even if the bounding box is behind the ray direction, it returns
     * true.
     */
    [[nodiscard]] bool intersectWithRay(const Ray &ray) const;
    [[nodiscard]] Vector3 lerp(const Vector3 &t) const;

    inline static Bounds3 createFromCenterSize(const Vector3 &center,
                                               const Vector3 &size)
    {
        return Bounds3(center - size * 0.5, center + size * 0.5);
    }
    static bool overlaps(const Bounds3 &lhs, const Bounds3 &rhs);
    static Bounds3 intersect(const Bounds3 &lhs, const Bounds3 &rhs);


    friend std::ostream &operator<<(std::ostream &out, const Bounds3 &bounds);
};
} // namespace kirana::math
#endif