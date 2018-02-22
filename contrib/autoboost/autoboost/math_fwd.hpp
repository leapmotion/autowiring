//  Boost math_fwd.hpp header file  ------------------------------------------//

//  (C) Copyright Hubert Holin and Daryle Walker 2001-2002.  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/math for documentation.

#ifndef AUTOBOOST_MATH_FWD_HPP
#define AUTOBOOST_MATH_FWD_HPP

#include <autoboost/cstdint.hpp>

namespace autoboost
{
namespace math
{


//  From <autoboost/math/quaternion.hpp>  ----------------------------------------//

template < typename T >
    class quaternion;

template < >
    class quaternion< float >;
template < >
    class quaternion< double >;
template < >
    class quaternion< long double >;

// Also has many function templates (including operators)


//  From <autoboost/math/octonion.hpp>  ------------------------------------------//

template < typename T >
    class octonion;

template < >
    class octonion< float >;
template < >
    class octonion< double >;
template < >
    class octonion< long double >;

// Also has many function templates (including operators)


//  From <autoboost/math/special_functions/acosh.hpp>  ---------------------------//

// Only has function template


//  From <autoboost/math/special_functions/asinh.hpp>  ---------------------------//

// Only has function template


//  From <autoboost/math/special_functions/atanh.hpp>  ---------------------------//

// Only has function template


//  From <autoboost/math/special_functions/sinc.hpp>  ----------------------------//

// Only has function templates


//  From <autoboost/math/special_functions/sinhc.hpp>  ---------------------------//

// Only has function templates


//  From <autoboost/math/common_factor.hpp>  -------------------------------------//

// Only #includes other headers


//  From <autoboost/math/common_factor_ct.hpp>  ----------------------------------//

#ifdef AUTOBOOST_NO_INTEGRAL_INT64_T
     typedef unsigned long static_gcd_type;
#else
     typedef autoboost::uintmax_t static_gcd_type;
#endif

template < static_gcd_type Value1, static_gcd_type Value2 >
    struct static_gcd;
template < static_gcd_type Value1, static_gcd_type Value2 >
    struct static_lcm;

}  // namespace math
}  // namespace autoboost


#endif  // AUTOBOOST_MATH_FWD_HPP
