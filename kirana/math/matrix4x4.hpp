#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include "vector4.hpp"
#include <iostream>

namespace kirana::math
{
class Vector3;
class Quaternion;
/**
 * Row-Major (storage) 4x4 Matrix. First three columns represent the basis
 * vectors x,y and z. Translation is stored in the fourth column ([i][3]).
 *
 * Note: Transpose the final matrix when using it with GLSL shaders. It expects
 * a column-major matrix.
 */
class Matrix4x4
{
  private:
    Vector4 m_current[4]{
        Vector4{1.0f, 0.0f, 0.0f, 0.0f},
        Vector4{0.0f, 1.0f, 0.0f, 0.0f},
        Vector4{0.0f, 0.0f, 1.0f, 0.0f},
        Vector4{0.0f, 0.0f, 0.0f, 1.0f},
    };

    [[nodiscard]] static float m_determinant2x2(float m00, float m01, float m10,
                                                float m11);
    [[nodiscard]] static float m_determinant3x3(float m00, float m01, float m02,
                                                float m10, float m11, float m12,
                                                float m20, float m21,
                                                float m22);

  public:
    static const Matrix4x4 IDENTITY;

    Matrix4x4() = default;
    explicit Matrix4x4(float matrix[4][4]);
    explicit Matrix4x4(float m00, float m01, float m02, float m03, float m10,
                       float m11, float m12, float m13, float m20, float m21,
                       float m22, float m23, float m30, float m31, float m32,
                       float m33);
    explicit Matrix4x4(Vector4 rows[4]);
    explicit Matrix4x4(const Vector4 &row0, const Vector4 &row1,
                       const Vector4 &row2, const Vector4 &row3);
    ~Matrix4x4() = default;

    Matrix4x4(const Matrix4x4 &mat);
    Matrix4x4 &operator=(const Matrix4x4 &mat);

    const Vector4 &operator[](size_t i) const;
    Vector4 &operator[](size_t i);

    bool operator==(const Matrix4x4 &mat) const;
    bool operator!=(const Matrix4x4 &mat) const;

    explicit operator std::string() const;

    Matrix4x4 &operator*=(const Matrix4x4 &rhs);

    friend Matrix4x4 operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    friend Vector4 operator*(const Matrix4x4 &mat, const Vector4 &vec4);
    friend Vector3 operator*(const Matrix4x4 &mat, const Vector3 &vec3);
    friend std::ostream &operator<<(std::ostream &out, const Matrix4x4 &mat);

    [[nodiscard]] float determinant() const;

    // Matrix functions
    static Matrix4x4 transpose(const Matrix4x4 &mat);
    static Matrix4x4 multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2);
    static Matrix4x4 inverse(const Matrix4x4 &mat);

    // Transformation functions
    /**
     * Decomposes the given transformation matrix into individual transformation
     * components.
     * @param mat The matrix to decompose. The matrix should not have any
     * projection component.
     * @param translation Pointer to Vector3 object to which translation will be
     * written out to.
     * @param scale Pointer to Vector3 object to which scale will be
     * written out to.
     * @param skew Pointer to Vector3 object to which skew will be
     * written out to.
     * @param rotation Pointer to Vector3 object to which rotation (euler angles
     * in degrees) will be written out to.
     * @return true if it succeeds in decomposing.
     */
    static bool decompose(const Matrix4x4 &mat, Vector3 *translation,
                          Vector3 *scale = nullptr, Vector3 *skew = nullptr,
                          Vector3 *rotation = nullptr);
    /**
     * Decomposes the given transformation matrix into individual transformation
     * components.
     * @param mat The matrix to decompose. The matrix should only have
     * translation and rotation components.
     * @param translation Pointer to Vector3 object to which translation will be
     * written out to.
     * @param rotation Pointer to Quaternion object to which rotation will be
     * written out to.
     * @return true if it succeeds in decomposing.
     */
    static bool decompose(const Matrix4x4 &mat, Vector3 *translation,
                          Quaternion *rotation);
    /**
     * Decomposes the given transformation matrix into individual transformation
     * components.
     * @param mat The matrix to decompose.
     * @param perspective Pointer to Vector4 object to which perspective
     * component will be written out to.
     * @param translation Pointer to Vector3 object to which translation will be
     * written out to.
     * @param scale Pointer to Vector3 object to which scale will be
     * written out to.
     * @param skew Pointer to Vector3 object to which skew will be
     * written out to.
     * @param rotation Pointer to Vector3 object to which rotation (euler angles
     * in degrees) will be written out to.
     * @return true if it succeeds in decomposing.
     */
    static bool decomposeProjection(const Matrix4x4 &mat, Vector4 *perspective,
                                    Vector3 *translation = nullptr,
                                    Vector3 *scale = nullptr,
                                    Vector3 *skew = nullptr,
                                    Vector3 *rotation = nullptr);

    static Matrix4x4 translation(const Vector3 &translation);
    static Matrix4x4 rotationX(float angle);
    static Matrix4x4 rotationY(float angle);
    static Matrix4x4 rotationZ(float angle);
    static Matrix4x4 rotation(const Vector3 &rotation);
    static Matrix4x4 rotation(const Vector3 &axis, float angle);
    static Matrix4x4 scale(const Vector3 &scale);
    static Matrix4x4 view(const Vector3 &eyePosition,
                          const Vector3 &lookAtPosition, const Vector3 &up);
    static Matrix4x4 view(const Vector3 &position, const Vector3 &forward,
                          const Vector3 &right, const Vector3 &up);

    // Projection Transformation functions
    static Matrix4x4 orthographicProjection(float left, float right,
                                            float bottom, float top, float near,
                                            float far, bool graphicsAPI = false,
                                            bool flipY = false);
    static Matrix4x4 orthographicProjection(float size, float aspectRatio,
                                            float near, float far,
                                            bool graphicsAPI = false,
                                            bool flipY = false);
    static Matrix4x4 perspectiveProjection(float fov, float aspectRatio,
                                           float near, float far,
                                           bool graphicsAPI = false,
                                           bool flipY = false);
};
} // namespace kirana::math

#endif