#pragma once
#include "type_mat1x1.h"

namespace glm
{
  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::col_type& mat<1, 1, T, Q>::operator[](length_type i)
  {
    assert(i < length());
    return value[i];
  }

  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::col_type const& mat<1, 1, T, Q>::operator[](length_type i) const
  {
    assert(i < length());
    return value[i];
  }

  template <typename T, qualifier Q>
  template <qualifier P>
  mat<1, 1, T, Q>::mat(mat<1, 1, T, P> const& m)
  {
    value[0] = m.value[0];
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>::mat(U const& x)
  {
    value[0][0] = x;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>::mat(vec<1, U, Q> const& v1)
  {
    value[0] = 1;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator+=(U s)
  {
    return value[0][0] += s;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator+=(mat<1, 1, U, Q> const& m)
  {
    return value[0][0] += m.value[0][0];
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator-=(U s)
  {
    return value[0][0] -= s;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator-=(mat<1, 1, U, Q> const& m)
  {
    return value[0][0] -= m.value[0][0];
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator*=(U s)
  {
    return value[0][0] *= s;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator*=(mat<1, 1, U, Q> const& m)
  {
    return value[0][0] *= m.value[0][0];
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator/=(U s)
  {
    return value[0][0] /= s;
  }

  template <typename T, qualifier Q>
  template <typename U>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator/=(mat<1, 1, U, Q> const& m)
  {
    return value[0][0] /= m.value[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator++()
  {
    return ++value[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q>& mat<1, 1, T, Q>::operator--()
  {
    return --value[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> mat<1, 1, T, Q>::operator++(int)
  {
    return value[0][0]++;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> mat<1, 1, T, Q>::operator--(int)
  {
    return value[0][0]--;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m)
  {
    return m;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m)
  {
    return -m[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m, T scalar)
  {
    return m[0][0] + scalar;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator+(T scalar, mat<1, 1, T, Q> const& m)
  {
    return scalar + m[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] + m2[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m, T scalar)
  {
    return m[0][0] - scalar;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator-(T scalar, mat<1, 1, T, Q> const& m)
  {
    return scalar - m[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] - m2[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator*(mat<1, 1, T, Q> const& m, T scalar)
  {
    return m[0][0] * scalar;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator*(T scalar, mat<1, 1, T, Q> const& m)
  {
    return scalar * m[0][0];
  }

  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::col_type operator*(mat<1, 1, T, Q> const& m, typename mat<1, 1, T, Q>::row_type const& v)
  {
    return { m[0][0] * v[0]};
  }

  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::row_type operator*(typename mat<1, 1, T, Q>::col_type const& v, mat<1, 1, T, Q> const& m)
  {
    return { v[0] * m[0][0] };
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator*(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] * m2[0][0];
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator/(mat<1, 1, T, Q> const& m, T scalar)
  {
    return m[0][0] / scalar;
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator/(T scalar, mat<1, 1, T, Q> const& m)
  {
    return scalar / m[0][0];
  }

  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::col_type operator/(mat<1, 1, T, Q> const& m, typename mat<1, 1, T, Q>::row_type const& v)
  {
    return { m[0][0] / v[0] };
  }

  template <typename T, qualifier Q>
  typename mat<1, 1, T, Q>::row_type operator/(typename mat<1, 1, T, Q>::col_type const& v, mat<1, 1, T, Q> const& m)
  {
    return { v[0] / m[0][0] };
  }

  template <typename T, qualifier Q>
  mat<1, 1, T, Q> operator/(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] / m2[0][0];
  }

  template <typename T, qualifier Q>
  bool operator==(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] == m2[0][0];
  }

  template <typename T, qualifier Q>
  bool operator!=(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2)
  {
    return m1[0][0] != m2[0][0];
  }
}
