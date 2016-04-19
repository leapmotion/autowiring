
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_POD_HPP_INCLUDED
#define AUTOBOOST_TT_IS_POD_HPP_INCLUDED

#include <autoboost/type_traits/detail/config.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/is_scalar.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

#ifdef __SUNPRO_CC
#include <autoboost/type_traits/is_function.hpp>
#endif

#include <cstddef>

#ifndef AUTOBOOST_IS_POD
#define AUTOBOOST_INTERNAL_IS_POD(T) false
#else
#define AUTOBOOST_INTERNAL_IS_POD(T) AUTOBOOST_IS_POD(T)
#endif

namespace autoboost {

// forward declaration, needed by 'is_pod_array_helper' template below
template< typename T > struct is_POD;

template <typename T> struct is_pod
: public integral_constant<bool, ::autoboost::is_scalar<T>::value || ::autoboost::is_void<T>::value || AUTOBOOST_INTERNAL_IS_POD(T)>
{};

#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <typename T, std::size_t sz> struct is_pod<T[sz]> : public is_pod<T>{};
#endif


// the following help compilers without partial specialization support:
template<> struct is_pod<void> : public true_type{};

#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template<> struct is_pod<void const> : public true_type{};
template<> struct is_pod<void const volatile> : public true_type{};
template<> struct is_pod<void volatile> : public true_type{};
#endif

template<class T> struct is_POD : public is_pod<T>{};

} // namespace autoboost

#undef AUTOBOOST_INTERNAL_IS_POD

#endif // AUTOBOOST_TT_IS_POD_HPP_INCLUDED
