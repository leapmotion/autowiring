
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_ARG_HPP_INCLUDED
#define AUTOBOOST_MPL_ARG_HPP_INCLUDED

// Copyright Peter Dimov 2001-2002
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
#   include <autoboost/mpl/arg_fwd.hpp>
#   include <autoboost/mpl/aux_/na.hpp>
#   include <autoboost/mpl/aux_/na_assert.hpp>
#   include <autoboost/mpl/aux_/arity_spec.hpp>
#   include <autoboost/mpl/aux_/arg_typedef.hpp>
#endif

#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER arg.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/config/lambda.hpp>
#   include <autoboost/mpl/aux_/config/dtp.hpp>
#   include <autoboost/mpl/aux_/nttp_decl.hpp>

#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

// local macro, #undef-ined at the end of the header
#if !defined(AUTOBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define AUX778076_ARG_N_DEFAULT_PARAMS(param,value) \
    AUTOBOOST_MPL_PP_DEFAULT_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        , value \
        ) \
    /**/
#else
#   define AUX778076_ARG_N_DEFAULT_PARAMS(param,value) \
    AUTOBOOST_MPL_PP_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        ) \
    /**/
#endif

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/arg.hpp>))
#include AUTOBOOST_PP_ITERATE()


#   undef AUX778076_ARG_N_DEFAULT_PARAMS

AUTOBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(1,int,arg)

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_ARG_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#if i_ > 0

template<> struct arg<i_>
{
    AUTOBOOST_STATIC_CONSTANT(int, value = i_);
    typedef arg<AUTOBOOST_PP_INC(i_)> next;
    AUTOBOOST_MPL_AUX_ARG_TYPEDEF(na, tag)
    AUTOBOOST_MPL_AUX_ARG_TYPEDEF(na, type)

    template<
          AUX778076_ARG_N_DEFAULT_PARAMS(typename U, na)
        >
    struct apply
    {
        typedef AUTOBOOST_PP_CAT(U,i_) type;
        AUTOBOOST_MPL_AUX_ASSERT_NOT_NA(type);
    };
};

#else

template<> struct arg<-1>
{
    AUTOBOOST_STATIC_CONSTANT(int, value = -1);
    AUTOBOOST_MPL_AUX_ARG_TYPEDEF(na, tag)
    AUTOBOOST_MPL_AUX_ARG_TYPEDEF(na, type)

    template<
          AUX778076_ARG_N_DEFAULT_PARAMS(typename U, na)
        >
    struct apply
    {
        typedef U1 type;
        AUTOBOOST_MPL_AUX_ASSERT_NOT_NA(type);
    };
};

#endif // i_ > 0

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
