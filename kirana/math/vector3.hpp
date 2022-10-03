#ifndef VECTOR3_HPP
#define VECTOR3_HPP


#include <iostream>


namespace kirana::math
{
class Vector4;
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

    float &x = m_current[0];
    float &y = m_current[1];
    float &z = m_current[2];

    Vector3() = default;
    ~Vector3() = default;
    explicit Vector3(float x, float y, float z);
    explicit Vector3(const Vector4 &vec4);

    Vector3(const Vector3 &vec3);
    Vector3 &operator=(const Vector3 &vec3);

    /// Cast to Vector4
    explicit operator Vector4();

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
    [[nodiscard]] Vector3 normalize() const;

    static float dot(const Vector3 &v, const Vector3 &w);
    static Vector3 cross(const Vector3 &v, const Vector3 &w);
    static Vector3 normalize(const Vector3 &v);
    static Vector3 lerp(const Vector3 &v, const Vector3 &w, float t);
    static Vector3 reflect(const Vector3 &v, const Vector3 &normal);
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
