
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_APPLY_FWD_HPP_INCLUDED
#define AUTOBOOST_MPL_APPLY_FWD_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER apply_fwd.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/nttp_decl.hpp>

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/cat.hpp>

// agurt, 15/jan/02: top-level 'apply' template gives an ICE on MSVC
// (for known reasons)
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
#   define AUTOBOOST_MPL_CFG_NO_APPLY_TEMPLATE
#endif

namespace autoboost { namespace mpl {

// local macro, #undef-ined at the end of the header
#   define AUX778076_APPLY_DEF_PARAMS(param, value) \
    AUTOBOOST_MPL_PP_DEFAULT_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        , value \
        ) \
    /**/

#   define AUX778076_APPLY_N_COMMA_PARAMS(n, param) \
    AUTOBOOST_PP_COMMA_IF(n) \
    AUTOBOOST_MPL_PP_PARAMS(n, param) \
    /**/

#   if !defined(AUTOBOOST_MPL_CFG_NO_APPLY_TEMPLATE)

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
// forward declaration
template<
      typename F, AUX778076_APPLY_DEF_PARAMS(typename T, na)
    >
struct apply;
#else
namespace aux {
template< AUTOBOOST_AUX_NTTP_DECL(int, arity_) > struct apply_chooser;
}
#endif

#   endif // AUTOBOOST_MPL_CFG_NO_APPLY_TEMPLATE

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/apply_fwd.hpp>))
#include AUTOBOOST_PP_ITERATE()


#   undef AUX778076_APPLY_N_COMMA_PARAMS
#   undef AUX778076_APPLY_DEF_PARAMS

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_APPLY_FWD_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<
      typename F AUX778076_APPLY_N_COMMA_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply,i_);

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
