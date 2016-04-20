//  (C) Copyright John Maddock 2007.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_COMPLEX_HPP
#define AUTOBOOST_TT_IS_COMPLEX_HPP

#include <autoboost/type_traits/is_convertible.hpp>
#include <complex>
// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>


namespace autoboost {
namespace detail{

struct is_convertible_from_tester
{
   template <class T>
   is_convertible_from_tester(const std::complex<T>&);
};

}

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_complex,T,(::autoboost::is_convertible<T, autoboost::detail::is_convertible_from_tester>::value))

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif //AUTOBOOST_TT_IS_COMPLEX_HPP
