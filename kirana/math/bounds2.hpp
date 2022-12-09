#ifndef BOUNDS_2_HPP
#define BOUNDS_2_HPP

#include "vector2.hpp"

namespace kirana::math
{
class Ray;
class Transform;
class Bounds2
{
    friend class Transform;

  private:
    Vector2 m_min;
    Vector2 m_max;

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
        return (m_min + m_max) * 0.5f;
    }
    [[nodiscard]] inline Vector2 getSize() const
    {
        return m_max - m_min;
    }
    [[nodiscard]] inline Vector2 getExtent() const
    {
        return getSize() * 0.5f;
    }

    void encapsulate(const Vector2 &point);
    void encapsulate(const Bounds2 &bounds);
    void expand(float delta);
    [[nodiscard]] bool contains(const Vector2 &point) const;
    [[nodiscard]] Vector2 lerp(const Vector2 &t) const;

    inline static Bounds2 createFromCenterSize(const Vector2 &center,
                                        const Vector2 &size)
    {
        return Bounds2(center - size * 0.5, center + size * 0.5);
    }
    static bool overlaps(const Bounds2 &lhs, const Bounds2 &rhs);
    static Bounds2 intersect(const Bounds2 &lhs, const Bounds2 &rhs);


    friend std::ostream &operator<<(std::ostream &out, const Bounds2 &bounds);
};
} // namespace kirana::math
#endif