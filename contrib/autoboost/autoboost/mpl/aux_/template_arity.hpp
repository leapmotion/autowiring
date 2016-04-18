
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/ttp.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/aux_/template_arity_fwd.hpp>
#   include <autoboost/mpl/int.hpp>
#   if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   if defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING)
#       include <autoboost/mpl/aux_/type_wrapper.hpp>
#   endif
#   else
#       include <autoboost/mpl/aux_/has_rebind.hpp>
#   endif
#endif

#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER template_arity.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   if defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING)

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/range.hpp>
#   include <autoboost/mpl/aux_/preprocessor/repeat.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/nttp_decl.hpp>

#   include <autoboost/preprocessor/seq/fold_left.hpp>
#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUX778076_ARITY AUTOBOOST_PP_INC(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY)

namespace autoboost { namespace mpl { namespace aux {

template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, N) > struct arity_tag
{
    typedef char (&type)[N + 1];
};

#   define AUX778076_MAX_ARITY_OP(unused, state, i_) \
    ( AUTOBOOST_PP_CAT(C,i_) > 0 ? AUTOBOOST_PP_CAT(C,i_) : state ) \
/**/

template<
      AUTOBOOST_MPL_PP_PARAMS(AUX778076_ARITY, AUTOBOOST_MPL_AUX_NTTP_DECL(int, C))
    >
struct max_arity
{
    AUTOBOOST_STATIC_CONSTANT(int, value =
          AUTOBOOST_PP_SEQ_FOLD_LEFT(
              AUX778076_MAX_ARITY_OP
            , -1
            , AUTOBOOST_MPL_PP_RANGE(1, AUX778076_ARITY)
            )
        );
};

#   undef AUX778076_MAX_ARITY_OP

arity_tag<0>::type arity_helper(...);

#   define AUTOBOOST_PP_ITERATION_LIMITS (1, AUX778076_ARITY)
#   define AUTOBOOST_PP_FILENAME_1 <autoboost/mpl/aux_/template_arity.hpp>
#   include AUTOBOOST_PP_ITERATE()

template< typename F, AUTOBOOST_MPL_AUX_NTTP_DECL(int, N) >
struct template_arity_impl
{
    AUTOBOOST_STATIC_CONSTANT(int, value =
          sizeof(::autoboost::mpl::aux::arity_helper(type_wrapper<F>(),arity_tag<N>())) - 1
        );
};

#   define AUX778076_TEMPLATE_ARITY_IMPL_INVOCATION(unused, i_, F) \
    AUTOBOOST_PP_COMMA_IF(i_) template_arity_impl<F,AUTOBOOST_PP_INC(i_)>::value \
/**/

template< typename F >
struct template_arity
{
    AUTOBOOST_STATIC_CONSTANT(int, value = (
          max_arity< AUTOBOOST_MPL_PP_REPEAT(
              AUX778076_ARITY
            , AUX778076_TEMPLATE_ARITY_IMPL_INVOCATION
            , F
            ) >::value
        ));

    typedef mpl::int_<value> type;
};

#   undef AUX778076_TEMPLATE_ARITY_IMPL_INVOCATION

#   undef AUX778076_ARITY

}}}

#   endif // AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING
#   else // AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#   include <autoboost/mpl/aux_/config/eti.hpp>

namespace autoboost { namespace mpl { namespace aux {

template< bool >
struct template_arity_impl
{
    template< typename F > struct result_
        : mpl::int_<-1>
    {
    };
};

template<>
struct template_arity_impl<true>
{
    template< typename F > struct result_
        : F::arity
    {
    };
};

template< typename F >
struct template_arity
    : template_arity_impl< ::autoboost::mpl::aux::has_rebind<F>::value >
        ::template result_<F>
{
};

#if defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)
template<>
struct template_arity<int>
    : mpl::int_<-1>
{
};
#endif

}}}

#   endif // AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<
      template< AUTOBOOST_MPL_PP_PARAMS(i_, typename P) > class F
    , AUTOBOOST_MPL_PP_PARAMS(i_, typename T)
    >
typename arity_tag<i_>::type
arity_helper(type_wrapper< F<AUTOBOOST_MPL_PP_PARAMS(i_, T)> >, arity_tag<i_>);

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
