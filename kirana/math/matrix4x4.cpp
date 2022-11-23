#include "math_utils.hpp"
#include "quaternion.hpp"

#include <algorithm>

using kirana::math::Matrix4x4;
using kirana::math::Vector3;
using kirana::math::Vector4;

const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4();

float Matrix4x4::m_determinant2x2(float m00, float m01, float m10, float m11)
{
    // | m00    m01|
    // | m10    m11|
    return m00 * m11 - m01 * m10;
}

float Matrix4x4::m_determinant3x3(float m00, float m01, float m02, float m10,
                                  float m11, float m12, float m20, float m21,
                                  float m22)
{
    // | m00    m01     m02|
    // | m10    m11     m12|
    // | m20    m21     m22|
    return m00 * m_determinant2x2(m11, m12, m21, m22) -
           m01 * m_determinant2x2(m10, m12, m20, m22) +
           m02 * m_determinant2x2(m10, m11, m20, m21);
}


Matrix4x4::Matrix4x4(float matrix[4][4])
{
    std::copy(&matrix[0][0], &matrix[0][0] + 16, &m_current[0][0]);
}

Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03, float m10,
                     float m11, float m12, float m13, float m20, float m21,
                     float m22, float m23, float m30, float m31, float m32,
                     float m33)
    : m_current{
          Vector4{m00, m01, m02, m03},
          Vector4{m10, m11, m12, m13},
          Vector4{m20, m21, m22, m23},
          Vector4{m30, m31, m32, m33},
      }
{
}


Matrix4x4::Matrix4x4(Vector4 rows[4])
    : m_current{rows[0], rows[1], rows[2], rows[3]}
{
}

Matrix4x4::Matrix4x4(Vector4 row0, Vector4 row1, Vector4 row2, Vector4 row3)
    : m_current{
          row0,
          row1,
          row2,
          row3,
      }
{
}

Matrix4x4::Matrix4x4(const Matrix4x4 &mat)
{
    if (&mat != this)
    {
        m_current[0] = mat[0];
        m_current[1] = mat[1];
        m_current[2] = mat[2];
        m_current[3] = mat[3];
    }
}

Matrix4x4 &Matrix4x4::operator=(const Matrix4x4 &mat)
{
    if (&mat != this)
    {
        m_current[0] = mat[0];
        m_current[1] = mat[1];
        m_current[2] = mat[2];
        m_current[3] = mat[3];
    }
    return *this;
}

const Vector4 &Matrix4x4::operator[](size_t i) const
{
    return m_current[i];
}

Vector4 &Matrix4x4::operator[](size_t i)
{
    return m_current[i];
}


bool Matrix4x4::operator==(const Matrix4x4 &mat) const
{
    return m_current[0] == mat.m_current[0] &&
           m_current[1] == mat.m_current[1] &&
           m_current[2] == mat.m_current[2] && m_current[3] == mat.m_current[3];
}

bool Matrix4x4::operator!=(const Matrix4x4 &mat) const
{
    return m_current[0] != mat.m_current[0] ||
           m_current[1] != mat.m_current[1] ||
           m_current[2] != mat.m_current[2] || m_current[3] != mat.m_current[3];
}

Matrix4x4 &Matrix4x4::operator*=(const Matrix4x4 &rhs)
{
    Vector4 temp[4]{m_current[0], m_current[1], m_current[2], m_current[3]};
    m_current[0][0] = temp[0][0] * rhs[0][0] + temp[0][1] * rhs[1][0] +
                      temp[0][2] * rhs[2][0] + temp[0][3] * rhs[3][0];
    m_current[0][1] = temp[0][0] * rhs[0][1] + temp[0][1] * rhs[1][1] +
                      temp[0][2] * rhs[2][1] + temp[0][3] * rhs[3][1];
    m_current[0][2] = temp[0][0] * rhs[0][2] + temp[0][1] * rhs[1][2] +
                      temp[0][2] * rhs[2][2] + temp[0][3] * rhs[3][2];
    m_current[0][3] = temp[0][0] * rhs[0][3] + temp[0][1] * rhs[1][3] +
                      temp[0][2] * rhs[2][3] + temp[0][3] * rhs[3][3];

    m_current[1][0] = temp[1][0] * rhs[0][0] + temp[1][1] * rhs[1][0] +
                      temp[1][2] * rhs[2][0] + temp[1][3] * rhs[3][0];
    m_current[1][1] = temp[1][0] * rhs[0][1] + temp[1][1] * rhs[1][1] +
                      temp[1][2] * rhs[2][1] + temp[1][3] * rhs[3][1];
    m_current[1][2] = temp[1][0] * rhs[0][2] + temp[1][1] * rhs[1][2] +
                      temp[1][2] * rhs[2][2] + temp[1][3] * rhs[3][2];
    m_current[1][3] = temp[1][0] * rhs[0][3] + temp[1][1] * rhs[1][3] +
                      temp[1][2] * rhs[2][3] + temp[1][3] * rhs[3][3];

    m_current[2][0] = temp[2][0] * rhs[0][0] + temp[2][1] * rhs[1][0] +
                      temp[2][2] * rhs[2][0] + temp[2][3] * rhs[3][0];
    m_current[2][1] = temp[2][0] * rhs[0][1] + temp[2][1] * rhs[1][1] +
                      temp[2][2] * rhs[2][1] + temp[2][3] * rhs[3][1];
    m_current[2][2] = temp[2][0] * rhs[0][2] + temp[2][1] * rhs[1][2] +
                      temp[2][2] * rhs[2][2] + temp[2][3] * rhs[3][2];
    m_current[2][3] = temp[2][0] * rhs[0][3] + temp[2][1] * rhs[1][3] +
                      temp[2][2] * rhs[2][3] + temp[2][3] * rhs[3][3];

    m_current[3][0] = temp[3][0] * rhs[0][0] + temp[3][1] * rhs[1][0] +
                      temp[3][2] * rhs[2][0] + temp[3][3] * rhs[3][0];
    m_current[3][1] = temp[3][0] * rhs[0][1] + temp[3][1] * rhs[1][1] +
                      temp[3][2] * rhs[2][1] + temp[3][3] * rhs[3][1];
    m_current[3][2] = temp[3][0] * rhs[0][2] + temp[3][1] * rhs[1][2] +
                      temp[3][2] * rhs[2][2] + temp[3][3] * rhs[3][2];
    m_current[3][3] = temp[3][0] * rhs[0][3] + temp[3][1] * rhs[1][3] +
                      temp[3][2] * rhs[2][3] + temp[3][3] * rhs[3][3];
    return *this;
}

Matrix4x4 kirana::math::operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
    return Matrix4x4(mat1[0][0] * mat2[0][0] + mat1[0][1] * mat2[1][0] +
                         mat1[0][2] * mat2[2][0] + mat1[0][3] * mat2[3][0],
                     mat1[0][0] * mat2[0][1] + mat1[0][1] * mat2[1][1] +
                         mat1[0][2] * mat2[2][1] + mat1[0][3] * mat2[3][1],
                     mat1[0][0] * mat2[0][2] + mat1[0][1] * mat2[1][2] +
                         mat1[0][2] * mat2[2][2] + mat1[0][3] * mat2[3][2],
                     mat1[0][0] * mat2[0][3] + mat1[0][1] * mat2[1][3] +
                         mat1[0][2] * mat2[2][3] + mat1[0][3] * mat2[3][3],
                     mat1[1][0] * mat2[0][0] + mat1[1][1] * mat2[1][0] +
                         mat1[1][2] * mat2[2][0] + mat1[1][3] * mat2[3][0],
                     mat1[1][0] * mat2[0][1] + mat1[1][1] * mat2[1][1] +
                         mat1[1][2] * mat2[2][1] + mat1[1][3] * mat2[3][1],
                     mat1[1][0] * mat2[0][2] + mat1[1][1] * mat2[1][2] +
                         mat1[1][2] * mat2[2][2] + mat1[1][3] * mat2[3][2],
                     mat1[1][0] * mat2[0][3] + mat1[1][1] * mat2[1][3] +
                         mat1[1][2] * mat2[2][3] + mat1[1][3] * mat2[3][3],
                     mat1[2][0] * mat2[0][0] + mat1[2][1] * mat2[1][0] +
                         mat1[2][2] * mat2[2][0] + mat1[2][3] * mat2[3][0],
                     mat1[2][0] * mat2[0][1] + mat1[2][1] * mat2[1][1] +
                         mat1[2][2] * mat2[2][1] + mat1[2][3] * mat2[3][1],
                     mat1[2][0] * mat2[0][2] + mat1[2][1] * mat2[1][2] +
                         mat1[2][2] * mat2[2][2] + mat1[2][3] * mat2[3][2],
                     mat1[2][0] * mat2[0][3] + mat1[2][1] * mat2[1][3] +
                         mat1[2][2] * mat2[2][3] + mat1[2][3] * mat2[3][3],
                     mat1[3][0] * mat2[0][0] + mat1[3][1] * mat2[1][0] +
                         mat1[3][2] * mat2[2][0] + mat1[3][3] * mat2[3][0],
                     mat1[3][0] * mat2[0][1] + mat1[3][1] * mat2[1][1] +
                         mat1[3][2] * mat2[2][1] + mat1[3][3] * mat2[3][1],
                     mat1[3][0] * mat2[0][2] + mat1[3][1] * mat2[1][2] +
                         mat1[3][2] * mat2[2][2] + mat1[3][3] * mat2[3][2],
                     mat1[3][0] * mat2[0][3] + mat1[3][1] * mat2[1][3] +
                         mat1[3][2] * mat2[2][3] + mat1[3][3] * mat2[3][3]);
}

Vector4 kirana::math::operator*(const Matrix4x4 &mat, const Vector4 &vec4)
{
    return Vector4(mat[0][0] * vec4[0] + mat[0][1] * vec4[1] +
                       mat[0][2] * vec4[2] + mat[0][3] * vec4[3],
                   mat[1][0] * vec4[0] + mat[1][1] * vec4[1] +
                       mat[1][2] * vec4[2] + mat[1][3] * vec4[3],
                   mat[2][0] * vec4[0] + mat[2][1] * vec4[1] +
                       mat[2][2] * vec4[2] + mat[2][3] * vec4[3],
                   mat[3][0] * vec4[0] + mat[3][1] * vec4[1] +
                       mat[3][2] * vec4[2] + mat[3][3] * vec4[3]);
}

std::ostream &kirana::math::operator<<(std::ostream &out, const Matrix4x4 &mat)
{
    return out << "\n[" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2]
               << ", " << mat[0][3] << ", \n"
               << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", "
               << mat[1][3] << ", \n"
               << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ", "
               << mat[2][3] << ", \n"
               << mat[3][0] << ", " << mat[3][1] << ", " << mat[3][2] << ", "
               << mat[3][3] << "]";
}

float Matrix4x4::determinant() const
{
    const auto &m = m_current;
    // | m00    m01     m02     m03|
    // | m10    m11     m12     m13|
    // | m20    m21     m22     m23|
    // | m30    m31     m32     m33|
    return m[0][0] * m_determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1],
                                      m[2][2], m[2][3], m[3][1], m[3][2],
                                      m[3][3]) -
           m[0][1] * m_determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0],
                                      m[2][2], m[2][3], m[3][0], m[3][2],
                                      m[3][3]) +
           m[0][2] * m_determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0],
                                      m[2][1], m[2][3], m[3][0], m[3][1],
                                      m[3][3]) -
           m[0][3] * m_determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0],
                                      m[2][1], m[2][2], m[3][0], m[3][1],
                                      m[3][2]);
}

Matrix4x4 Matrix4x4::transpose(const Matrix4x4 &mat)
{
    return Matrix4x4(mat[0][0], mat[1][0], mat[2][0], mat[3][0], mat[0][1],
                     mat[1][1], mat[2][1], mat[3][1], mat[0][2], mat[1][2],
                     mat[2][2], mat[3][2], mat[0][3], mat[1][3], mat[2][3],
                     mat[3][3]);
}

Matrix4x4 Matrix4x4::multiply(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
    return mat1 * mat2;
}

Matrix4x4 Matrix4x4::inverse(const Matrix4x4 &mat)
{
    // Gauss Jordan Elimination method of finding inverse.

    int indxc[4], indxr[4];
    int ipiv[4] = {0, 0, 0, 0};
    Vector4 minv[4];
    minv[0] = mat[0];
    minv[1] = mat[1];
    minv[2] = mat[2];
    minv[3] = mat[3];
    for (int i = 0; i < 4; i++)
    {
        int irow = 0, icol = 0;
        float big = 0.0f;
        // Choose pivot
        for (int j = 0; j < 4; j++)
        {
            if (ipiv[j] != 1)
            {
                for (int k = 0; k < 4; k++)
                {
                    if (ipiv[k] == 0)
                    {
                        if (std::abs(minv[j][k]) >= big)
                        {
                            big = std::fabsf(minv[j][k]);
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1)
                        return Matrix4x4::IDENTITY; // Singular matrix, cannot
                                                    // find inverse.
                }
            }
        }
        ++ipiv[icol];
        // Swap rows _irow_ and _icol_ for pivot
        if (irow != icol)
        {
            for (int k = 0; k < 4; ++k)
                std::swap(minv[irow][k], minv[icol][k]);
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if (minv[icol][icol] == 0.f)
            return Matrix4x4::IDENTITY; // Singular matrix, cannot find inverse.

        // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
        float pivinv = 1.0f / minv[icol][icol];
        minv[icol][icol] = 1.;
        for (int j = 0; j < 4; j++)
            minv[icol][j] *= pivinv;

        // Subtract this row from others to zero out their columns
        for (int j = 0; j < 4; j++)
        {
            if (j != icol)
            {
                float save = minv[j][icol];
                minv[j][icol] = 0;
                for (int k = 0; k < 4; k++)
                    minv[j][k] -= minv[icol][k] * save;
            }
        }
    }
    // Swap columns to reflect permutation
    for (int j = 3; j >= 0; j--)
    {
        if (indxr[j] != indxc[j])
        {
            for (int k = 0; k < 4; k++)
                std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
        }
    }
    return Matrix4x4(minv);
}

bool Matrix4x4::decompose(const Matrix4x4 &mat, Vector3 *translation,
                          Vector3 *scale, Vector3 *skew, Vector3 *rotation)
{
    // Algorithm from Graphic Gems II: Decomposing a matrix into simple
    // transformations. By Spencer W. Thomas.
    // Link:
    // https://github.com/erich666/GraphicsGems/blob/master/gemsii/unmatrix.c

    // Extract translation
    if (translation != nullptr)
        *translation = Vector3(mat[0][3], mat[1][3], mat[2][3]);

    if (scale == nullptr && skew == nullptr && rotation == nullptr)
        return true;

    Vector3 xAxis = Vector3(mat[0][0], mat[1][0], mat[2][0]);
    Vector3 yAxis = Vector3(mat[0][1], mat[1][1], mat[2][1]);
    Vector3 zAxis = Vector3(mat[0][2], mat[1][2], mat[2][2]);

    // Extract scale and skew
    Vector3 tempScale;
    Vector3 tempSkew;
    tempScale[0] = xAxis.length();
    xAxis = xAxis / xAxis.length();

    tempSkew[2] = Vector3::dot(xAxis, yAxis);
    yAxis = yAxis + xAxis * -tempSkew[2];

    tempScale[1] = yAxis.length();
    yAxis = yAxis / yAxis.length();

    tempSkew[2] /= tempScale[1];

    tempSkew[1] = Vector3::dot(xAxis, zAxis);
    zAxis = zAxis + xAxis * -tempSkew[1];
    tempSkew[0] = Vector3::dot(yAxis, zAxis);
    zAxis = zAxis + yAxis * -tempSkew[0];

    tempScale[2] = zAxis.length();
    zAxis = zAxis / zAxis.length();

    tempSkew[1] /= tempScale[2];
    tempSkew[0] /= tempScale[2];

    if (Vector3::dot(xAxis, Vector3::cross(yAxis, zAxis)) < 0)
    {
        tempScale = -tempScale;
        xAxis = -xAxis;
        yAxis = -yAxis;
        zAxis = -zAxis;
    }

    if (scale != nullptr)
        *scale = tempScale;
    if (skew != nullptr)
        *skew = tempSkew;

    if (rotation == nullptr)
        return true;

    // Extract rotation
    Vector3 tempRotation;
    tempRotation[1] = std::asin(-xAxis[2]);
    if (!kirana::math::approximatelyEqual(std::cos(tempRotation[1]), 0.0f))
    {
        tempRotation[0] = std::atan2(yAxis[2], zAxis[2]);
        tempRotation[2] = std::atan2(xAxis[1], xAxis[0]);
    }
    else
    {
        tempRotation[0] = std::atan2(-zAxis[0], yAxis[1]);
        tempRotation[2] = 0.0f;
    }

    if (rotation != nullptr)
    {
        tempRotation[0] = math::degrees(tempRotation[0]);
        tempRotation[1] = math::degrees(tempRotation[1]);
        tempRotation[2] = math::degrees(tempRotation[2]);
        *rotation = tempRotation;
    }

    return true;
}

bool Matrix4x4::decompose(const Matrix4x4 &mat, Vector3 *translation,
                          Quaternion *rotation)
{
    // Extract translation
    if (translation != nullptr)
        *translation = Vector3(mat[0][3], mat[1][3], mat[2][3]);

    if (rotation == nullptr)
        return true;

    *rotation = Quaternion::matrix(mat);
    return true;
}

bool Matrix4x4::decomposeProjection(const Matrix4x4 &mat, Vector4 *perspective,
                                    Vector3 *translation, Vector3 *scale,
                                    Vector3 *skew, Vector3 *rotation)
{
    // Algorithm from Graphic Gems II: Decomposing a matrix into simple
    // transformations. By Spencer W. Thomas.
    // Link:
    // https://github.com/erich666/GraphicsGems/blob/master/gemsii/unmatrix.c

    Matrix4x4 tempMat(Matrix4x4::transpose(mat));

    if (approximatelyEqual(tempMat[3][3], 0.0f))
        return false;
    tempMat[0][0] /= tempMat[3][3];
    tempMat[0][1] /= tempMat[3][3];
    tempMat[0][2] /= tempMat[3][3];
    tempMat[0][3] /= tempMat[3][3];
    tempMat[1][0] /= tempMat[3][3];
    tempMat[1][1] /= tempMat[3][3];
    tempMat[1][2] /= tempMat[3][3];
    tempMat[1][3] /= tempMat[3][3];
    tempMat[2][0] /= tempMat[3][3];
    tempMat[2][1] /= tempMat[3][3];
    tempMat[2][2] /= tempMat[3][3];
    tempMat[2][3] /= tempMat[3][3];
    tempMat[3][0] /= tempMat[3][3];
    tempMat[3][1] /= tempMat[3][3];
    tempMat[3][2] /= tempMat[3][3];
    tempMat[3][3] /= tempMat[3][3];

    Matrix4x4 projMat(tempMat);
    projMat[3][0] = 0.0f;
    projMat[3][1] = 0.0f;
    projMat[3][2] = 0.0f;
    projMat[3][3] = 1.0f;

    if (approximatelyEqual(projMat.determinant(), 0.0f))
        return false;

    if (!approximatelyEqual(tempMat[3][0], 0.0f) ||
        !approximatelyEqual(tempMat[3][1], 0.0f) ||
        !approximatelyEqual(tempMat[3][2], 0.0f))
    {
        Vector4 rhs(tempMat[3][0], tempMat[3][1], tempMat[3][2], tempMat[3][3]);

        (*perspective) =
            Matrix4x4::transpose(Matrix4x4::inverse(projMat)) * rhs;

        tempMat[3][0] = 0.0f;
        tempMat[3][1] = 0.0f;
        tempMat[3][2] = 0.0f;
        tempMat[3][3] = 1.0f;
    }
    else
    {
        if (perspective != nullptr)
            (*perspective) = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }


    if (translation == nullptr && scale == nullptr && skew == nullptr &&
        rotation == nullptr)
        return true;
    return decompose(tempMat, translation, scale, skew, rotation);
}

Matrix4x4 Matrix4x4::translation(const Vector3 &translation)
{
    return Matrix4x4(1.0f, 0.0f, 0.0f, translation[0], 0.0f, 1.0f, 0.0f,
                     translation[1], 0.0f, 0.0f, 1.0f, translation[2], 0.0f,
                     0.0f, 0.0f, 1.0f);
}


Matrix4x4 Matrix4x4::rotationX(float angle)
{
    float cos = std::cosf(math::radians(angle));
    float sin = std::sinf(math::radians(angle));

    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos, -sin, 0.0f, 0.0f, sin,
                     cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::rotationY(float angle)
{
    float cos = std::cosf(math::radians(angle));
    float sin = std::sinf(math::radians(angle));

    return Matrix4x4(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin, 0.0f,
                     cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::rotationZ(float angle)
{
    float cos = std::cosf(math::radians(angle));
    float sin = std::sinf(math::radians(angle));

    return Matrix4x4(cos, -sin, 0.0f, 0.0f, sin, cos, 0.0f, 0.0f, 0.0f, 0.0f,
                     1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::rotation(const Vector3 &rotation)
{
    float cosX = std::cosf(math::radians(rotation[0]));
    float sinX = std::sinf(math::radians(rotation[0]));
    float cosY = std::cosf(math::radians(rotation[1]));
    float sinY = std::sinf(math::radians(rotation[1]));
    float cosZ = std::cosf(math::radians(rotation[2]));
    float sinZ = std::sinf(math::radians(rotation[2]));

    // Perform XYZ rotation (euler angles).
    return Matrix4x4(cosY * cosZ, sinX * sinY * cosZ - cosX * sinZ,
                     cosX * sinY * cosZ + sinX * sinZ, 0.0f, cosY * sinZ,
                     sinX * sinY * sinZ + cosX * cosZ,
                     cosX * sinY * sinZ - sinX * cosZ, 0.0f, -sinY, sinX * cosY,
                     cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}


Matrix4x4 Matrix4x4::rotation(const Vector3 &a, float angle)
{
    // Goldman, Ronald, “Matrices and Transformations,” in Andrew S. Glassner,
    // ed., Graphics Gems, Academic Press, pp. 472–475, 1990. Cited on p. 75
    // @link {https://dl.acm.org/doi/10.5555/90767.90908}

    float c = std::cosf(math::radians(angle));
    float omc = 1.0f - c;
    float s = std::sinf(math::radians(angle));
    return Matrix4x4(c + omc * a[0] * a[0], omc * a[0] * a[1] - a[2] * s,
                     omc * a[0] * a[2] + a[1] * s, 0.0f,
                     omc * a[0] * a[1] + a[2] * s, c + omc * a[1] * a[1],
                     omc * a[1] * a[2] - a[0] * s, 0.0f,
                     omc * a[0] * a[2] - a[1] * s, omc * a[1] * a[2] + a[0] * s,
                     c + omc * a[2] * a[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::scale(const Vector3 &scale)
{
    return Matrix4x4(scale[0], 0.0f, 0.0f, 0.0f, 0.0f, scale[1], 0.0f, 0.0f,
                     0.0f, 0.0f, scale[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}
Matrix4x4 Matrix4x4::orthographicProjection(float left, float right,
                                            float bottom, float top, float near,
                                            float far, bool graphicsAPI,
                                            bool flipY)
{
    float nf = graphicsAPI ? (far - near) : (near - far);

    return Matrix4x4(2.0f / (right - left), 0.0f, 0.0f,
                     -(right + left) / (right - left), 0.0f,
                     (flipY ? -1.0f : 0.0f) * 2.0f / (top - bottom), 0.0f,
                     -(top + bottom) / (top - bottom), 0.0f, 0.0f,
                     (graphicsAPI ? -1.0f : 1.0f) * 2.0f / nf,
                     -(near + far) / nf, 0.0f, 0.0f, 0.0f, 1.0f);

    // Inverse

    //    return Matrix4x4(0.5f * (right - left), 0.0f, 0.0f, 0.5f * (right +
    //    left),
    //                     0.0f, (flipY ? -1.0f : 0.0f) * 0.5f * (top - bottom),
    //                     0.0f, 0.5f * (top + bottom), 0.0f, 0.0f, (graphicsAPI
    //                     ? -1.0f : 1.0f) * 0.5f * nf, (graphicsAPI ? -1.0f
    //                     : 1.0f) * 0.5f * (near + far), 0.0f, 0.0f,
    //                     0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::orthographicProjection(float size, float aspectRatio,
                                            float near, float far,
                                            bool graphicsAPI, bool flipY)
{
    return orthographicProjection(
        -size / 2.0f, size / 2.0f, (-size / 2.0f) / aspectRatio,
        (size / 2.0f) / aspectRatio, near, far, graphicsAPI, flipY);
}

Matrix4x4 Matrix4x4::perspectiveProjection(float fov, float aspectRatio,
                                           float near, float far,
                                           bool graphicsAPI, bool flipY)
{
    float top = std::abs(near) * std::tan(math::radians(fov * 0.5f));
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;

    float lr = graphicsAPI ? right - left : left - right;
    float bt = graphicsAPI ? top - bottom : bottom - top;
    float nf = graphicsAPI ? far - near : near - far;

    return Matrix4x4(
        2.0f * near / (right - left), 0.0f, (left + right) / lr, 0.0f, 0.0f,
        (flipY ? -1.0f : 1.0f) * 2.0f * near / (top - bottom),
        (bottom + top) / bt, 0.0f, 0.0f, 0.0f,
        (graphicsAPI ? -1.0f : 1.0f) * (far + near) / nf,
        (graphicsAPI ? -1.0f : 1.0f) * 2.0f * far * near / (far - near), 0.0f,
        0.0f, (graphicsAPI ? -1.0f : 1.0f), 0.0f);

    // Inverse

    //    return Matrix4x4(
    //        0.5f * far * (right - left), 0.0f, 0.0f, 0.5f * far * (left +
    //        right), 0.0f, (flipY ? -1.0f : 1.0f) * 0.5f * far * (top -
    //        bottom), 0.0f, 0.5f * far * (bottom + top), 0.0f, 0.0f, 0.0f, far
    //        * near, 0.0f, 0.0f, 0.5f * (far - near), 0.5f * (-far - near) +
    //        far + near);
}