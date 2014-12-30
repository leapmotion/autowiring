
//  Copyright (c) 2014 Agustin Berge
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED
#define AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED

#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {
template <typename T> struct is_final_impl
{
#ifdef AUTOBOOST_IS_FINAL
   typedef typename remove_cv<T>::type cvt;
   AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_IS_FINAL(cvt));
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value = false);
#endif
};
} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_final,T,::autoboost::detail::is_final_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED
