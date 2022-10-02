#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

namespace kirana::math
{
class Vector4;
class Matrix4x4
{
  private:
    float m[4][4]{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    };

  public:
    static const Matrix4x4 IDENTITY;

    float &m00 = m[0][0];
    float &m01 = m[0][1];
    float &m02 = m[0][2];
    float &m03 = m[0][3];
    float &m10 = m[1][0];
    float &m11 = m[1][1];
    float &m12 = m[1][2];
    float &m13 = m[1][3];
    float &m20 = m[2][0];
    float &m21 = m[2][1];
    float &m22 = m[2][2];
    float &m23 = m[2][3];
    float &m30 = m[3][0];
    float &m31 = m[3][1];
    float &m32 = m[3][2];
    float &m33 = m[3][3];

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

    Vector4 operator[](int i) const;

    bool operator==(const Matrix4x4 &mat) const;
    bool operator!=(const Matrix4x4 &mat) const;

    friend Matrix4x4 operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);

    static Matrix4x4 transpose(const Matrix4x4 &mat);
    static Matrix4x4 multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    static Matrix4x4 inverse(const Matrix4x4 &mat);

};
} // namespace kirana::math

#endif