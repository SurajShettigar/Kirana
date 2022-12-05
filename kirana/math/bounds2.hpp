#ifndef BOUNDS_2_HPP
#define BOUNDS_2_HPP

#include "vector2.hpp"

namespace kirana::math
{
class Ray;
class Bounds2
{
  private:
    Vector2 m_min;
    Vector2 m_max;
    Vector2 m_center;
    Vector2 m_size;
    Vector2 m_extent;

  public:
    Bounds2();
    /**
     * Creates a bounding box enclosing the given point.
     * @param point The point to enclose.
     */
    explicit Bounds2(const Vector2 &point);
    explicit Bounds2(const Vector2 &min, const Vector2 &max);
    ~Bounds2() = default;

    Bounds2(const Bounds2 &bounds);
    Bounds2 &operator=(const Bounds2 &bounds);

    Vector2 operator[](int i) const
    {
        return (i == 0) ? m_min : m_max;
    }

    Vector2 &operator[](int i)
    {
        return (i == 0) ? m_min : m_max;
    }

    explicit operator std::string() const;

    // Getters-Setters
    [[nodiscard]] inline Vector2 getMin() const
    {
        return m_min;
    }
    [[nodiscard]] inline Vector2 getMax() const
    {
        return m_max;
    }
    [[nodiscard]] inline Vector2 getCenter() const
    {
        return m_center;
    }
    [[nodiscard]] inline Vector2 getSize() const
    {
        return m_size;
    }
    [[nodiscard]] inline Vector2 getExtent() const
    {
        return m_extent;
    }

    void encapsulate(const Vector2 &point);
    void encapsulate(const Bounds2 &bounds);
    void expand(float delta);
    bool contains(const Vector2 &point);
    Vector2 lerp(const Vector2 &t);

    static Bounds2 createFromCenterSize(const Vector2 &center,
                                        const Vector2 &size);
    static bool overlaps(const Bounds2 &lhs, const Bounds2 &rhs);
    static Bounds2 intersect(const Bounds2 &lhs, const Bounds2 &rhs);


    friend std::ostream &operator<<(std::ostream &out, const Bounds2 &bounds);
};
} // namespace kirana::math
#endif