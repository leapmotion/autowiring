////////////////////////////////////////////////////////////////////
//
// Copyright Vicente J. Botet Escriba 2010
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.
//
////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_MPL_ABS_HPP_INCLUDED
#define AUTOBOOST_MPL_ABS_HPP_INCLUDED

#include <autoboost/mpl/integral_c.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/config/integral.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>

#if    !defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC_2) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && !defined(__CUDACC__) \
    && ( defined(AUTOBOOST_MSVC) \
        || AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 238) \
        )

#   define AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC_2

#endif

namespace autoboost { namespace mpl {

template< typename Tag > struct abs_impl;

template< typename T > struct abs_tag
{
    typedef typename T::tag type;
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(N)
    >
struct abs
    : abs_impl<
          typename abs_tag<N>::type
        >::template apply<N>::type
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1, abs, (N))
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, abs)

template<
      typename T
    , T n1
    >
struct abs_c
    : abs<integral_c<T,n1> >
{
};

#if defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC_2)
namespace aux {
template< typename T, T n > struct abs_wknd
{
    AUTOBOOST_STATIC_CONSTANT(T, value = (n < 0 ? -n : n));
    typedef integral_c<T,value> type;
};
}
#endif

template<>
struct abs_impl<integral_c_tag>
{
#if defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC_2)
    template< typename N > struct apply
        : aux::abs_wknd< typename N::value_type, N::value >
#else
    template< typename N > struct apply
        : integral_c< typename N::value_type, ((N::value < 0) ? (-N::value) : N::value ) >
#endif
    {
    };
};

}}

#endif // AUTOBOOST_MPL_ABS_HPP_INCLUDED
