
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED
#define AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED

#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/detail/cv_traits_impl.hpp>
#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#include <cstddef>

// should be the last #include
#include <autoboost/type_traits/detail/type_trait_def.hpp>

namespace autoboost {


namespace detail {

template <typename T, bool is_vol>
struct remove_const_helper
{
    typedef T type;
};

template <typename T>
struct remove_const_helper<T, true>
{
    typedef T volatile type;
};


template <typename T>
struct remove_const_impl
{
    typedef typename remove_const_helper<
       typename cv_traits_imp<AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(T)>::unqualified_type
        , ::autoboost::is_volatile<T>::value
        >::type type;
};

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//
template <typename T>
struct remove_const_impl<T&&>
{
    typedef T&& type;
};
#endif

} // namespace detail

// * convert a type T to non-const type - remove_const<T>

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_const,T,typename autoboost::detail::remove_const_impl<T>::type)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_const,T&,T&)
#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_const,T const[N],T type[N])
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_const,T const volatile[N],T volatile type[N])
#endif


} // namespace autoboost

#include <autoboost/type_traits/detail/type_trait_undef.hpp>

#endif // AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED
