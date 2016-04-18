
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_BIND_FWD_HPP_INCLUDED
#define AUTOBOOST_MPL_BIND_FWD_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
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
#   include <autoboost/mpl/aux_/na.hpp>
#endif

#include <autoboost/mpl/aux_/config/bind.hpp>
#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER bind_fwd.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/config/dmc_ambiguous_ctps.hpp>

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/cat.hpp>

namespace autoboost { namespace mpl {

// local macros, #undef-ined at the end of the header

#   if defined(AUTOBOOST_MPL_CFG_DMC_AMBIGUOUS_CTPS)
#       define AUX778076_DMC_PARAM() , int dummy_ = 0
#   else
#       define AUX778076_DMC_PARAM()
#   endif

#   define AUX778076_BIND_DEFAULT_PARAMS(param, value) \
    AUTOBOOST_MPL_PP_DEFAULT_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        , value \
        ) \
    AUX778076_DMC_PARAM() \
    /**/

#   define AUX778076_BIND_N_PARAMS(n, param) \
    AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_MPL_PP_PARAMS(n, param) \
    AUX778076_DMC_PARAM() \
    /**/

#if !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE)
template<
      typename F, AUX778076_BIND_DEFAULT_PARAMS(typename T, na)
    >
struct bind;
#endif

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/bind_fwd.hpp>))
#include AUTOBOOST_PP_ITERATE()

#   undef AUX778076_BIND_N_PARAMS
#   undef AUX778076_BIND_DEFAULT_PARAMS
#   undef AUX778076_DMC_PARAM
}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_BIND_FWD_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(bind,i_);

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
