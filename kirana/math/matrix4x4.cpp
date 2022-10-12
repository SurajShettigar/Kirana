#include "matrix4x4.hpp"
#include "math_utils.hpp"
#include <algorithm>
#include "vector3.hpp"

using kirana::math::Matrix4x4;
using kirana::math::Vector4;

const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4();

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
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 4; j++)
            if (!approximatelyEqual(m_current[i][j], mat[i][j]))
                return false;
    return true;
}

bool Matrix4x4::operator!=(const Matrix4x4 &mat) const
{
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 4; j++)
            if (!approximatelyEqual(m_current[i][j], mat[i][j]))
                return true;
    return false;
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