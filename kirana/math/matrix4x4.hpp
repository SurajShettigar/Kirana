#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include <iostream>

namespace kirana::math
{
class Vector3;
class Vector4;
class Transform;
class Matrix4x4
{
    friend class Transform;

  private:
    float m_current[4][4]{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };

  public:
    static const Matrix4x4 IDENTITY;

    Matrix4x4() = default;
    explicit Matrix4x4(float matrix[4][4]);
    explicit Matrix4x4(float m00, float m01, float m02, float m03, float m10,
                       float m11, float m12, float m13, float m20, float m21,
                       float m22, float m23, float m30, float m31, float m32,
                       float m33);
    explicit Matrix4x4(Vector4 c0, Vector4 c1, Vector4 c2, Vector4 c3);
    ~Matrix4x4() = default;

    Matrix4x4(const Matrix4x4 &mat);
    Matrix4x4 &operator=(const Matrix4x4 &mat);

    Vector4 operator[](size_t i) const;

    bool operator==(const Matrix4x4 &mat) const;
    bool operator!=(const Matrix4x4 &mat) const;

    Matrix4x4 &operator*=(const Matrix4x4 &rhs);

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
        return &m_current[0][0];
    }

    friend Matrix4x4 operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    friend Vector4 operator*(const Matrix4x4 &mat, const Vector4 &vec4);
    friend std::ostream &operator<<(std::ostream &out, const Matrix4x4 &mat);

    static Matrix4x4 transpose(const Matrix4x4 &mat);
    static Matrix4x4 multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    static Matrix4x4 inverse(const Matrix4x4 &mat);
    static void rotate(Matrix4x4 *mat, float angle, const Vector3 &axis);
};
} // namespace kirana::math

#endif