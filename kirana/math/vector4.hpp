#ifndef VECTOR4_HPP
#define VECTOR4_HPP


#include <iostream>
#include <array>


namespace kirana::math
{
class Vector2;
class Vector3;
/**
 * Column 4D-Vector
 */
class Vector4
{
  private:
    std::array<float, 4> m_current{0, 0, 0, 0};

  public:
    // static values
    static const Vector4 ZERO;
    static const Vector4 ONE;

    Vector4() = default;
    ~Vector4() = default;
    explicit Vector4(std::array<float, 4> vector);
    explicit Vector4(float x, float y, float z, float w);
    explicit Vector4(const Vector2 &vec2, float z = 0.0f, float w = 0.0f);
    explicit Vector4(const Vector3 &vec3, float w = 0.0f);

    Vector4(const Vector4 &vec4);
    Vector4 &operator=(const Vector4 &vec4);
    bool operator==(const Vector4 &rhs) const;
    bool operator!=(const Vector4 &rhs) const;

    /// Cast to Vector2
    explicit operator Vector2() const;
    /// Cast to Vector3
    explicit operator Vector3() const;
    /// Cast to string
    explicit operator std::string() const;

    inline const float &operator[](int i) const
    {
        return i < 0 ? m_current[0] : (i > 3 ? m_current[3] : m_current.at(i));
    }
    inline float &operator[](int i)
    {
        return i < 0 ? m_current[0] : (i > 3 ? m_current[3] : m_current.at(i));
    }

    // Current Vector operations
    Vector4 operator-() const;
    Vector4 &operator+=(const Vector4 &rhs);
    Vector4 &operator-=(const Vector4 &rhs);
    Vector4 &operator*=(float rhs);
    Vector4 &operator/=(float rhs);

    // Vector Specific operations
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthSquared() const;
    void normalize();

    [[nodiscard]] inline size_t size() const
    {
        return sizeof(m_current);
    }

    [[nodiscard]] inline const float *data() const
    {
        return m_current.data();
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
