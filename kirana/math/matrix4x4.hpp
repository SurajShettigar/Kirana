#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

namespace kirana::math
{
class Vector4;
class Matrix4x4
{
  private:
    float m_current[4][4]{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };

  public:
    static const Matrix4x4 IDENTITY;

    float &m00 = m_current[0][0];
    float &m01 = m_current[0][1];
    float &m02 = m_current[0][2];
    float &m03 = m_current[0][3];
    float &m10 = m_current[1][0];
    float &m11 = m_current[1][1];
    float &m12 = m_current[1][2];
    float &m13 = m_current[1][3];
    float &m20 = m_current[2][0];
    float &m21 = m_current[2][1];
    float &m22 = m_current[2][2];
    float &m23 = m_current[2][3];
    float &m30 = m_current[3][0];
    float &m31 = m_current[3][1];
    float &m32 = m_current[3][2];
    float &m33 = m_current[3][3];

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

    friend Matrix4x4 operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    friend Vector4 operator*(const Matrix4x4 &mat, const Vector4 &vec4);

    static Matrix4x4 transpose(const Matrix4x4 &mat);
    static Matrix4x4 multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    static Matrix4x4 inverse(const Matrix4x4 &mat);
};
} // namespace kirana::math

#endif