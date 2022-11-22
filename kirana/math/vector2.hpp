#ifndef VECTOR2_HPP
#define VECTOR2_HPP


#include <iostream>


namespace kirana::math
{
class Vector3;
class Vector4;
/**
 * Column 2D-Vector
 */
class Vector2
{
  private:
    float m_current[2]{0, 0};

  public:
    // static values
    static const Vector2 ZERO;
    static const Vector2 ONE;
    static const Vector2 UP;
    static const Vector2 DOWN;
    static const Vector2 LEFT;
    static const Vector2 RIGHT;

    Vector2() = default;
    ~Vector2() = default;
    explicit Vector2(float x, float y);
    explicit Vector2(const Vector3 &vec3);
    explicit Vector2(const Vector4 &vec4);

    Vector2(const Vector2 &vec2);
    Vector2 &operator=(const Vector2 &vec2);

    /// Cast to Vector3
    explicit operator Vector3() const;
    /// Cast to Vector4
    explicit operator Vector4() const;

    inline float operator[](size_t i) const
    {
        return m_current[i];
    }
    inline float &operator[](size_t i)
    {
        return m_current[i];
    }

    // Current Vector operations
    Vector2 operator-() const;
    Vector2 &operator+=(const Vector2 &rhs);
    Vector2 &operator-=(const Vector2 &rhs);
    Vector2 &operator*=(float rhs);
    Vector2 &operator/=(float rhs);
    bool operator==(const Vector2 &rhs) const;
    bool operator!=(const Vector2 &rhs) const;

    // Vector Specific operations
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthSquared() const;
    [[nodiscard]] Vector2 normalize() const;

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
        return &m_current[0];
    }

    static float dot(const Vector2 &v, const Vector2 &w);
    static Vector2 normalize(const Vector2 &v);
    static Vector2 lerp(const Vector2 &v, const Vector2 &w, float t);

    // Arithmetic operations
    friend Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs);
    friend Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs);
    friend Vector2 operator*(const Vector2 &lhs, float rhs);
    friend Vector2 operator*(float lhs, const Vector2 &rhs);
    friend Vector2 operator/(const Vector2 &lhs, float rhs);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Vector2 &vec2);
};

} // namespace kirana::math

#endif // VECTOR2_HPP
