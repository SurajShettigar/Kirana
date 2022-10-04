#include "matrix4x4.hpp"
#include "vector4.hpp"
#include "math_utils.hpp"
#include <algorithm>

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
          {m00, m01, m02, m03},
          {m10, m11, m12, m13},
          {m20, m21, m22, m23},
          {m30, m31, m32, m33},
      }
{
}

Matrix4x4::Matrix4x4(Vector4 c0, Vector4 c1, Vector4 c2, Vector4 c3)
    : m_current{
          {c0.x, c1.x, c2.x, c3.x},
          {c0.y, c1.y, c2.y, c3.y},
          {c0.z, c1.z, c2.z, c3.z},
          {c0.w, c1.w, c2.w, c3.w},
      }
{
}

Matrix4x4::Matrix4x4(const Matrix4x4 &mat)
{
    if (&mat != this)
        std::copy(&mat.m_current[0][0], &mat.m_current[0][0] + 16,
                  &m_current[0][0]);
}

Matrix4x4 &Matrix4x4::operator=(const Matrix4x4 &mat)
{
    if (&mat != this)
        std::copy(&mat.m_current[0][0], &mat.m_current[0][0] + 16,
                  &m_current[0][0]);
    return *this;
}


Vector4 Matrix4x4::operator[](size_t i) const
{
    return Vector4(m_current[0][i], m_current[1][i], m_current[2][i],
                   m_current[3][i]);
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
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 4; j++)
            m_current[i][j] = m_current[i][0] * rhs.m_current[0][j] +
                              m_current[i][1] * rhs.m_current[1][j] +
                              m_current[i][2] * rhs.m_current[2][j] +
                              m_current[i][3] * rhs.m_current[3][j];
    return *this;
}

Matrix4x4 kirana::math::operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
    Matrix4x4 mul = mat1;
    mul *= mat2;
    return mul;
}

Vector4 kirana::math::operator*(const Matrix4x4 &mat, const Vector4 &vec4)
{
    return Vector4(mat.m00 * vec4.x + mat.m01 * vec4.y + mat.m02 * vec4.z +
                       mat.m03 * vec4.w,
                   mat.m10 * vec4.x + mat.m11 * vec4.y + mat.m12 * vec4.z +
                       mat.m13 * vec4.w,
                   mat.m20 * vec4.x + mat.m21 * vec4.y + mat.m22 * vec4.z +
                       mat.m23 * vec4.w,
                   mat.m30 * vec4.x + mat.m31 * vec4.y + mat.m32 * vec4.z +
                       mat.m33 * vec4.w);
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
    float minv[4][4];
    std::copy(&mat.m_current[0][0], &mat.m_current[0][0] + 16, &minv[0][0]);
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