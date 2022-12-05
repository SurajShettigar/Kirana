#ifndef BOUNDS_3_HPP
#define BOUNDS_3_HPP

#include "vector3.hpp"

namespace kirana::math
{
class Ray;
class Bounds3
{
  private:
    Vector3 m_min;
    Vector3 m_max;
    Vector3 m_center;
    Vector3 m_size;
    Vector3 m_extent;

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
        return m_center;
    }
    [[nodiscard]] inline Vector3 getSize() const
    {
        return m_size;
    }
    [[nodiscard]] inline Vector3 getExtent() const
    {
        return m_extent;
    }

    void encapsulate(const Vector3 &point);
    void encapsulate(const Bounds3 &bounds);
    void expand(float delta);
    bool contains(const Vector3 &point);
    bool intersectWithRay(const Ray &ray, Vector3 *enterPoint,
                          Vector3 *exitPoint);
    /**
     * Faster version of ray-bounding-box intersection.
     * @param ray
     * @return true if the entire ray is inside the bounding box.
     * i.e. even if the bounding box is behind the ray direction, it returns
     * true.
     */
    bool intersectWithRay(const Ray &ray);
    Vector3 lerp(const Vector3 &t);

    static Bounds3 createFromCenterSize(const Vector3 &center,
                                        const Vector3 &size);
    static bool overlaps(const Bounds3 &lhs, const Bounds3 &rhs);
    static Bounds3 intersect(const Bounds3 &lhs, const Bounds3 &rhs);


    friend std::ostream &operator<<(std::ostream &out, const Bounds3 &bounds);
};
} // namespace kirana::math
#endif