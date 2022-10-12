#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include "vector4.hpp"
#include <iostream>

namespace kirana::math
{
class Vector3;
class Matrix4x4
{
  private:
    Vector4 m_current[4]{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f},
        Vector4{0.0f, 0.0f, 0.0f, 1.0f},
    };

  public:
    static const Matrix4x4 IDENTITY;

    Matrix4x4() = default;
    explicit Matrix4x4(float matrix[4][4]);
    explicit Matrix4x4(float m00, float m01, float m02, float m03, float m10,
                       float m11, float m12, float m13, float m20, float m21,
                       float m22, float m23, float m30, float m31, float m32,
                       float m33);
    explicit Matrix4x4(Vector4 rows[4]);
    explicit Matrix4x4(Vector4 row0, Vector4 row1, Vector4 row2, Vector4 row3);
    ~Matrix4x4() = default;

    Matrix4x4(const Matrix4x4 &mat);
    Matrix4x4 &operator=(const Matrix4x4 &mat);

    const Vector4 &operator[](size_t i) const;
    Vector4 &operator[](size_t i);

    bool operator==(const Matrix4x4 &mat) const;
    bool operator!=(const Matrix4x4 &mat) const;

    Matrix4x4 &operator*=(const Matrix4x4 &rhs);

    friend Matrix4x4 operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    friend Vector4 operator*(const Matrix4x4 &mat, const Vector4 &vec4);
    friend std::ostream &operator<<(std::ostream &out, const Matrix4x4 &mat);

    static Matrix4x4 transpose(const Matrix4x4 &mat);
    static Matrix4x4 multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    static Matrix4x4 inverse(const Matrix4x4 &mat);
};
} // namespace kirana::math

#endif