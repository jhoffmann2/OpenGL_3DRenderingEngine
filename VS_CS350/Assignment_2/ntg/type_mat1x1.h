#pragma once

#include "glm/detail/type_vec2.hpp"
#include "glm/detail/type_mat.hpp"
#include <limits>
#include <cstddef>

namespace glm
{
	template<typename T, qualifier Q>
	struct mat<1, 1, T, Q>
	{
		typedef vec<1, T, Q> col_type;
		typedef vec<1, T, Q> row_type;
		typedef mat<1, 1, T, Q> type;
		typedef mat<1, 1, T, Q> transpose_type;
		typedef T value_type;

	private:
		col_type value[1];

	public:
		// -- Accesses --

		typedef length_t length_type;
		static constexpr length_type length() { return 1; }

		col_type& operator[](length_type i);
		col_type const& operator[](length_type i) const;

		// -- Constructors --

		mat() = default;
		mat(mat<1, 1, T, Q> const& m) = default;
		template<qualifier P>
		mat(mat<1, 1, T, P> const& m);

		// -- Conversions --

		template<typename U>
		mat(U const& x);

		template<typename U>
		mat(vec<1, U, Q> const& v1);

		// -- Unary arithmetic operators --

		mat<1, 1, T, Q>& operator=(mat<1, 1, T, Q> const& v) = default;

		template<typename U>
		mat<1, 1, T, Q>& operator+=(U s);
		template<typename U>
		mat<1, 1, T, Q>& operator+=(mat<1, 1, U, Q> const& m);
		template<typename U>
		mat<1, 1, T, Q>& operator-=(U s);
		template<typename U>
		mat<1, 1, T, Q>& operator-=(mat<1, 1, U, Q> const& m);
		template<typename U>
		mat<1, 1, T, Q>& operator*=(U s);
		template<typename U>
		mat<1, 1, T, Q>& operator*=(mat<1, 1, U, Q> const& m);
		template<typename U>
		mat<1, 1, T, Q>& operator/=(U s);
		template<typename U>
		mat<1, 1, T, Q>& operator/=(mat<1, 1, U, Q> const& m);

		// -- Increment and decrement operators --

		mat<1, 1, T, Q>& operator++ ();
		mat<1, 1, T, Q>& operator-- ();
		mat<1, 1, T, Q> operator++(int);
		mat<1, 1, T, Q> operator--(int);
	};

	// -- Unary operators --

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m);

	// -- Binary operators --

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m, T scalar);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator+(T scalar, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator+(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m, T scalar);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator-(T scalar, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator-(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator*(mat<1, 1, T, Q> const& m, T scalar);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator*(T scalar, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	typename mat<1, 1, T, Q>::col_type operator*(mat<1, 1, T, Q> const& m, typename mat<1, 1, T, Q>::row_type const& v);

	template<typename T, qualifier Q>
	typename mat<1, 1, T, Q>::row_type operator*(typename mat<1, 1, T, Q>::col_type const& v, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator*(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator/(mat<1, 1, T, Q> const& m, T scalar);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator/(T scalar, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	typename mat<1, 1, T, Q>::col_type operator/(mat<1, 1, T, Q> const& m, typename mat<1, 1, T, Q>::row_type const& v);

	template<typename T, qualifier Q>
	typename mat<1, 1, T, Q>::row_type operator/(typename mat<1, 1, T, Q>::col_type const& v, mat<1, 1, T, Q> const& m);

	template<typename T, qualifier Q>
	mat<1, 1, T, Q> operator/(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	// -- Boolean operators --

	template<typename T, qualifier Q>
	bool operator==(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	template<typename T, qualifier Q>
	bool operator!=(mat<1, 1, T, Q> const& m1, mat<1, 1, T, Q> const& m2);

	namespace detail
	{
	  template<typename T, qualifier Q, bool Aligned>
	  struct compute_determinant<1, 1, T, Q, Aligned>
	  {
		  GLM_FUNC_QUALIFIER static T call(mat<1, 1, T, Q> const& m)
		  {
			  return m[0][0];
		  }
	  };
	}
	
}
