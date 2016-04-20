
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_UNION_HPP_INCLUDED
#define AUTOBOOST_TT_IS_UNION_HPP_INCLUDED

#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {
#ifndef __GNUC__
template <typename T> struct is_union_impl
{
   typedef typename remove_cv<T>::type cvt;
#ifdef AUTOBOOST_IS_UNION
   AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_IS_UNION(cvt));
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value = false);
#endif
};
#else
//
// using remove_cv here generates a whole load of needless
// warnings with gcc, since it doesn't do any good with gcc
// in any case (at least at present), just remove it:
//
template <typename T> struct is_union_impl
{
#ifdef AUTOBOOST_IS_UNION
   AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_IS_UNION(T));
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value = false);
#endif
};
#endif
} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_union,T,::autoboost::detail::is_union_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_UNION_HPP_INCLUDED
