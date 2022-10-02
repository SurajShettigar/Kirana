#ifndef VECTOR4_HPP
#define VECTOR4_HPP


#include <iostream>


namespace kirana::math
{
class Vector3;
class Vector4
{
  private:
    float v[4]{0, 0, 0, 0};

  public:
    float &x = v[0];
    float &y = v[1];
    float &z = v[2];
    float &w = v[3];

    Vector4() = default;
    ~Vector4() = default;
    explicit Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f,
                     float w = 0.0f);
    explicit Vector4(const Vector3 &vec3, float w = 0.0f);

    Vector4(const Vector4 &w);
    Vector4 &operator=(const Vector4 &w);

    /// Cast to Vector3
    explicit operator Vector3();

    inline float operator[](int i) const
    {
        return v[i];
    }
    inline float &operator[](int i)
    {
        return v[i];
    }

    // Current Vector operations
    Vector4 operator-() const;
    Vector4 &operator+=(const Vector4 &w);
    Vector4 &operator-=(const Vector4 &w);
    Vector4 &operator*=(float a);
    Vector4 &operator/=(float a);

    // Vector Specific operations
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthSquared() const;
    [[nodiscard]] Vector4 normalize() const;

    static Vector4 normalize(const Vector4 &v);
    static Vector4 lerp(const Vector4 &v, const Vector4 &w, float t);

    // Arithmetic operations
    friend Vector4 operator+(const Vector4 &v, const Vector4 &w);
    friend Vector4 operator-(const Vector4 &v, const Vector4 &w);
    friend Vector4 operator*(const Vector4 &v, float a);
    friend Vector4 operator*(float a, const Vector4 &v);
    friend Vector4 operator/(const Vector4 &v, float a);

    // Other operations
    friend std::ostream &operator<<(std::ostream &out, const Vector4 &v);

    // Comparison operations
    friend bool operator==(const Vector4 &v, const Vector4 &w);
    friend bool operator!=(const Vector4 &v, const Vector4 &w);
};

} // namespace kirana::math


#endif // VECTOR4_HPP
