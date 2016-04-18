
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_POD_HPP_INCLUDED
#define AUTOBOOST_TT_IS_POD_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/is_scalar.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

#include <cstddef>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

#ifndef AUTOBOOST_IS_POD
#define AUTOBOOST_INTERNAL_IS_POD(T) false
#else
#define AUTOBOOST_INTERNAL_IS_POD(T) AUTOBOOST_IS_POD(T)
#endif

namespace autoboost {

// forward declaration, needed by 'is_pod_array_helper' template below
template< typename T > struct is_POD;

namespace detail {


template <typename T> struct is_pod_impl
{
    AUTOBOOST_STATIC_CONSTANT(
        bool, value =
        (::autoboost::type_traits::ice_or<
            ::autoboost::is_scalar<T>::value,
            ::autoboost::is_void<T>::value,
            AUTOBOOST_INTERNAL_IS_POD(T)
         >::value));
};

#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <typename T, std::size_t sz>
struct is_pod_impl<T[sz]>
    : public is_pod_impl<T>
{
};
#endif


// the following help compilers without partial specialization support:
AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void,true)

#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void volatile,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const volatile,true)
#endif

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_pod,T,::autoboost::detail::is_pod_impl<T>::value)
// is_POD is the old depricated name for this trait, do not use this as it may
// be removed in future without warning!!
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_POD,T,::autoboost::is_pod<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#undef AUTOBOOST_INTERNAL_IS_POD

#endif // AUTOBOOST_TT_IS_POD_HPP_INCLUDED
