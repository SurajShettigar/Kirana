#ifndef VECTOR3_HPP
#define VECTOR3_HPP


#include <iostream>


namespace kirana::math
{
class Vector4;
class Vector3
{
  private:
    float v[3]{0, 0, 0};

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

    float &x = v[0];
    float &y = v[1];
    float &z = v[2];

    Vector3() = default;
    ~Vector3() = default;
    explicit Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    explicit Vector3(const Vector4 &vec4);

    Vector3(const Vector3 &w);
    Vector3 &operator=(const Vector3 &w);

    /// Cast to Vector4
    explicit operator Vector4();

    inline float operator[](int i) const
    {
        return v[i];
    }
    inline float &operator[](int i)
    {
        return v[i];
    }

    // Current Vector operations
    Vector3 operator-() const;
    Vector3 &operator+=(const Vector3 &w);
    Vector3 &operator-=(const Vector3 &w);
    Vector3 &operator*=(float a);
    Vector3 &operator/=(float a);

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
    friend Vector3 operator+(const Vector3 &v, const Vector3 &w);
    friend Vector3 operator-(const Vector3 &v, const Vector3 &w);
    friend Vector3 operator*(const Vector3 &v, float a);
    friend Vector3 operator*(float a, const Vector3 &v);
    friend Vector3 operator/(const Vector3 &v, float a);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Vector3 &v);

    // Comparison operations
    friend bool operator==(const Vector3 &v, const Vector3 &w);
    friend bool operator!=(const Vector3 &v, const Vector3 &w);
};

} // namespace kirana::math

#endif // VECTOR3_HPP
