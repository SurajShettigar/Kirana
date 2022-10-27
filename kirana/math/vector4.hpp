#ifndef VECTOR4_HPP
#define VECTOR4_HPP


#include <iostream>


namespace kirana::math
{
class Vector2;
class Vector3;
class Vector4
{
  private:
    float m_current[4]{0, 0, 0, 0};

  public:
    // static values
    static const Vector4 ZERO;
    static const Vector4 ONE;

    Vector4() = default;
    ~Vector4() = default;
    explicit Vector4(float x, float y, float z, float w);
    explicit Vector4(const Vector2 &vec2, float z = 0.0f, float w = 0.0f);
    explicit Vector4(const Vector3 &vec3, float w = 0.0f);

    Vector4(const Vector4 &vec4);
    Vector4 &operator=(const Vector4 &vec4);

    /// Cast to Vector2
    explicit operator Vector2() const;
    /// Cast to Vector3
    explicit operator Vector3() const;

    inline const float &operator[](size_t i) const
    {
        return m_current[i];
    }
    inline float &operator[](size_t i)
    {
        return m_current[i];
    }

    // Current Vector operations
    Vector4 operator-() const;
    Vector4 &operator+=(const Vector4 &rhs);
    Vector4 &operator-=(const Vector4 &rhs);
    Vector4 &operator*=(float rhs);
    Vector4 &operator/=(float rhs);
    bool operator==(const Vector4 &rhs) const;
    bool operator!=(const Vector4 &rhs) const;

    // Vector Specific operations
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthSquared() const;
    [[nodiscard]] Vector4 normalize() const;

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

    static Vector4 normalize(const Vector4 &vec4);
    static Vector4 lerp(const Vector4 &v, const Vector4 &w, float t);

    // Arithmetic operations
    friend Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs);
    friend Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs);
    friend Vector4 operator*(const Vector4 &lhs, float rhs);
    friend Vector4 operator*(float lhs, const Vector4 &rhs);
    friend Vector4 operator/(const Vector4 &lhs, float rhs);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Vector4 &vec4);
};

} // namespace kirana::math


#endif // VECTOR4_HPP
