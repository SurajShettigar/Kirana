#ifndef VECTOR3_HPP
#define VECTOR3_HPP


#include <iostream>


namespace kirana::math
{
class Vector2;
class Vector4;
/**
 * Column 3D-Vector
 */
class Vector3
{
  private:
    float m_current[3]{0, 0, 0};

  public:
    // static values
    static const Vector3 ZERO;
    static const Vector3 ONE;
    static const Vector3 UP;
    static const Vector3 DOWN;
    static const Vector3 LEFT;
    static const Vector3 RIGHT;
    static const Vector3 FORWARD;
    static const Vector3 BACK;

    Vector3() = default;
    ~Vector3() = default;
    explicit Vector3(float x, float y, float z);
    explicit Vector3(const Vector2 &vec2);
    explicit Vector3(const Vector4 &vec4);

    Vector3(const Vector3 &vec3);
    Vector3 &operator=(const Vector3 &vec3);

    /// Cast to Vector2
    explicit operator Vector2() const;
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
    Vector3 operator-() const;
    Vector3 &operator+=(const Vector3 &rhs);
    Vector3 &operator-=(const Vector3 &rhs);
    Vector3 &operator*=(float rhs);
    Vector3 &operator/=(float rhs);
    bool operator==(const Vector3 &rhs) const;
    bool operator!=(const Vector3 &rhs) const;

    // Vector Specific operations
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthSquared() const;
    void normalize();

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

    static float dot(const Vector3 &v, const Vector3 &w);
    static Vector3 cross(const Vector3 &v, const Vector3 &w);
    static Vector3 normalize(const Vector3 &v);
    static Vector3 lerp(const Vector3 &v, const Vector3 &w, float t);
    static Vector3 reflect(const Vector3 &v, const Vector3 &normal);
    static Vector3 spherical(const Vector3 &v, float radius=1.0f);
    static void coordinateFrame(const Vector3 &w, Vector3 *u, Vector3 *v);

    // Arithmetic operations
    friend Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs);
    friend Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs);
    friend Vector3 operator*(const Vector3 &lhs, float rhs);
    friend Vector3 operator*(float lhs, const Vector3 &rhs);
    friend Vector3 operator/(const Vector3 &lhs, float rhs);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Vector3 &vec3);
};

} // namespace kirana::math

#endif // VECTOR3_HPP
