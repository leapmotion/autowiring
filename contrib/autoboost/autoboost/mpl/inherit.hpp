
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_INHERIT_HPP_INCLUDED
#define AUTOBOOST_MPL_INHERIT_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/empty_base.hpp>
#   include <autoboost/mpl/aux_/na_spec.hpp>
#   include <autoboost/mpl/aux_/lambda_support.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER inherit.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/config/dtp.hpp>

#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/dec.hpp>
#   include <autoboost/preprocessor/cat.hpp>

namespace autoboost { namespace mpl {

// 'inherit<T1,T2,..,Tn>' metafunction; returns an unspecified class type
// produced by public derivation from all metafunction's parameters
// (T1,T2,..,Tn), except the parameters of 'empty_base' class type;
// regardless the position and number of 'empty_base' parameters in the
// metafunction's argument list, derivation from them is always a no-op;
// for instance:
//      inherit<her>::type == her
//      inherit<her,my>::type == struct unspecified : her, my {};
//      inherit<empty_base,her>::type == her
//      inherit<empty_base,her,empty_base,empty_base>::type == her
//      inherit<her,empty_base,my>::type == struct unspecified : her, my {};
//      inherit<empty_base,empty_base>::type == empty_base

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T2)
    >
struct inherit2
    : T1, T2
{
    typedef inherit2 type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2, inherit2, (T1,T2))
};

template< typename T1 >
struct inherit2<T1,empty_base>
{
    typedef T1 type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2, inherit2, (T1,empty_base))
};

template< typename T2 >
struct inherit2<empty_base,T2>
{
    typedef T2 type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2, inherit2, (empty_base,T2))
};

// needed to disambiguate the previous two in case when both
// T1 and T2 == empty_base
template<>
struct inherit2<empty_base,empty_base>
{
    typedef empty_base type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2, inherit2, (empty_base,empty_base))
};

#else

namespace aux {

template< bool C1, bool C2 >
struct inherit2_impl
{
    template< typename Derived, typename T1, typename T2 > struct result_
        : T1, T2
    {
        typedef Derived type_;
    };
};

template<>
struct inherit2_impl<false,true>
{
    template< typename Derived, typename T1, typename T2 > struct result_
        : T1
    {
        typedef T1 type_;
    };
};

template<>
struct inherit2_impl<true,false>
{
    template< typename Derived, typename T1, typename T2 > struct result_
        : T2
    {
        typedef T2 type_;
    };
};

template<>
struct inherit2_impl<true,true>
{
    template< typename Derived, typename T1, typename T2 > struct result_
    {
        typedef T1 type_;
    };
};

} // namespace aux

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T2)
    >
struct inherit2
    : aux::inherit2_impl<
          is_empty_base<T1>::value
        , is_empty_base<T2>::value
        >::template result_< inherit2<T1,T2>,T1,T2 >
{
    typedef typename inherit2::type_ type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2, inherit2, (T1,T2))
};

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

AUTOBOOST_MPL_AUX_NA_SPEC(2, inherit2)

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(3, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/inherit.hpp>))
#include AUTOBOOST_PP_ITERATE()

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_INHERIT_HPP_INCLUDED

///// iteration

#else
#define n_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n_, typename T, na)
    >
struct AUTOBOOST_PP_CAT(inherit,n_)
    : inherit2<
          typename AUTOBOOST_PP_CAT(inherit,AUTOBOOST_PP_DEC(n_))<
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(n_), T)
            >::type
        , AUTOBOOST_PP_CAT(T,n_)
        >
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(
          n_
        , AUTOBOOST_PP_CAT(inherit,n_)
        , (AUTOBOOST_MPL_PP_PARAMS(n_, T))
        )
};

AUTOBOOST_MPL_AUX_NA_SPEC(n_, AUTOBOOST_PP_CAT(inherit,n_))

#if n_ == AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
/// primary template
template<
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n_, typename T, empty_base)
    >
struct inherit
    : AUTOBOOST_PP_CAT(inherit,n_)<AUTOBOOST_MPL_PP_PARAMS(n_, T)>
{
};

// 'na' specialization
template<>
struct inherit< AUTOBOOST_MPL_PP_ENUM(5, na) >
{
    template<
#if !defined(AUTOBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
          AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n_, typename T, empty_base)
#else
          AUTOBOOST_MPL_PP_PARAMS(n_, typename T)
#endif
        >
    struct apply
        : inherit< AUTOBOOST_MPL_PP_PARAMS(n_, T) >
    {
    };
};

AUTOBOOST_MPL_AUX_NA_SPEC_LAMBDA(n_, inherit)
AUTOBOOST_MPL_AUX_NA_SPEC_ARITY(n_, inherit)
AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(n_, n_, inherit)
#endif

#undef n_
#endif // AUTOBOOST_PP_IS_ITERATING
