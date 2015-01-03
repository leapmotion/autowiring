
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_REMOVE_ALL_EXTENTS_HPP_INCLUDED
#define AUTOBOOST_TT_REMOVE_ALL_EXTENTS_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <cstddef>
#include <autoboost/detail/workaround.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/type_trait_def.hpp>

namespace autoboost {

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_all_extents,T,T)

#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T[N],typename autoboost::remove_all_extents<T>::type type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T const[N],typename autoboost::remove_all_extents<T const>::type type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T volatile[N],typename autoboost::remove_all_extents<T volatile>::type type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T const volatile[N],typename autoboost::remove_all_extents<T const volatile>::type type)
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) && !defined(__IBMCPP__) &&  !AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T[],typename autoboost::remove_all_extents<T>::type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T const[],typename autoboost::remove_all_extents<T const>::type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T volatile[],typename autoboost::remove_all_extents<T volatile>::type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T const volatile[],typename autoboost::remove_all_extents<T const volatile>::type)
#endif
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/type_trait_undef.hpp>

#endif // AUTOBOOST_TT_REMOVE_BOUNDS_HPP_INCLUDED
