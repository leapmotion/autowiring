
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000-2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_REMOVE_EXTENT_HPP_INCLUDED
#define AUTOBOOST_TT_REMOVE_EXTENT_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>
#include <cstddef>

// should be the last #include
#include <autoboost/type_traits/detail/type_trait_def.hpp>

namespace autoboost {

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_extent,T,T)

#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T[N],T type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T const[N],T const type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T volatile[N],T volatile type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T const volatile[N],T const volatile type)
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) && !defined(__IBMCPP__) &&  !AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T[],T)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T const[],T const)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T volatile[],T volatile)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T const volatile[],T const volatile)
#endif
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/type_trait_undef.hpp>

#endif // AUTOBOOST_TT_REMOVE_BOUNDS_HPP_INCLUDED
