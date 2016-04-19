
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_APPLY_WRAP_HPP_INCLUDED
#define AUTOBOOST_MPL_APPLY_WRAP_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
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
#   include <autoboost/mpl/aux_/arity.hpp>
#   include <autoboost/mpl/aux_/has_apply.hpp>
#   include <autoboost/mpl/aux_/na.hpp>
#   include <autoboost/mpl/aux_/msvc_never_true.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER apply_wrap.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#   include <autoboost/mpl/aux_/preprocessor/add.hpp>
#   include <autoboost/mpl/aux_/config/bcc.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/config/dtp.hpp>
#   include <autoboost/mpl/aux_/config/eti.hpp>
#   include <autoboost/mpl/aux_/config/msvc.hpp>
#   include <autoboost/mpl/aux_/config/workaround.hpp>

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/logical/and.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/iterate.hpp>


namespace autoboost { namespace mpl {

// local macros, #undef-ined at the end of the header
#   define AUX778076_APPLY_WRAP_PARAMS(n, param) \
    AUTOBOOST_MPL_PP_PARAMS(n, param) \
    /**/

#   define AUX778076_APPLY_WRAP_SPEC_PARAMS(n, param) \
    AUTOBOOST_MPL_PP_ENUM(AUTOBOOST_PP_INC(n), param) \
    /**/


#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/apply_wrap.hpp>))
#include AUTOBOOST_PP_ITERATE()


#   undef AUX778076_APPLY_WRAP_SPEC_PARAMS
#   undef AUX778076_APPLY_WRAP_PARAMS

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_APPLY_WRAP_HPP_INCLUDED

///// iteration, depth == 1

// For gcc 4.4 compatability, we must include the
// AUTOBOOST_PP_ITERATION_DEPTH test inside an #else clause.
#else // AUTOBOOST_PP_IS_ITERATING
#if AUTOBOOST_PP_ITERATION_DEPTH() == 1

#   define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#   if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
// MSVC version

#define AUX778076_MSVC_DTW_NAME AUTOBOOST_PP_CAT(msvc_apply,i_)
#define AUX778076_MSVC_DTW_ORIGINAL_NAME apply
#define AUX778076_MSVC_DTW_ARITY i_
#include <autoboost/mpl/aux_/msvc_dtw.hpp>

template<
      typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply_wrap,i_)
{
    // Metafunction forwarding confuses vc6
    typedef typename AUTOBOOST_PP_CAT(msvc_apply,i_)<F>::template result_<
          AUX778076_APPLY_WRAP_PARAMS(i_, T)
        >::type type;
};

#   elif defined(AUTOBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
// MWCW/Borland version

template<
      int N, typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply_wrap_impl,i_);

#define AUTOBOOST_PP_ITERATION_PARAMS_2 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY - i_, <autoboost/mpl/apply_wrap.hpp>))
#include AUTOBOOST_PP_ITERATE()

template<
      typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply_wrap,i_)
    : AUTOBOOST_PP_CAT(apply_wrap_impl,i_)<
          ::autoboost::mpl::aux::arity<F,i_>::value
        , F
        AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, T)
        >::type
{
};

#   else
// ISO98 C++, with minor concession to vc7

template<
      typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
#if i_ == 0
    , typename has_apply_ = typename aux::has_apply<F>::type
#endif
    >
struct AUTOBOOST_PP_CAT(apply_wrap,i_)
// metafunction forwarding confuses MSVC 7.0
#if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)
    : F::template apply< AUX778076_APPLY_WRAP_PARAMS(i_, T) >
{
#else
{
    typedef typename F::template apply<
         AUX778076_APPLY_WRAP_PARAMS(i_, T)
        >::type type;
#endif
};

#if i_ == 0 && !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
template< typename F >
struct AUTOBOOST_PP_CAT(apply_wrap,i_)<F,true_>
    : F::apply
{
};
#endif

#   endif // workarounds

#if defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)
/// workaround for ETI bug
template<>
struct AUTOBOOST_PP_CAT(apply_wrap,i_)<AUX778076_APPLY_WRAP_SPEC_PARAMS(i_, int)>
{
    typedef int type;
};
#endif

#   undef i_

///// iteration, depth == 2

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 2

#   define j_ AUTOBOOST_PP_FRAME_ITERATION(2)

#if i_ == 0 && j_ == 0 \
    && defined(AUTOBOOST_MPL_CFG_BCC590_WORKAROUNDS) \
    && !defined(AUTOBOOST_MPL_CFG_NO_HAS_APPLY)

template< typename F, bool F_has_apply >
struct apply_wrap_impl0_bcb {
    typedef typename F::template apply< na > type;
};

template< typename F >
struct apply_wrap_impl0_bcb< F, true > {
    typedef typename F::apply type;
};

template<
      typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply_wrap_impl,i_)<
          AUTOBOOST_MPL_PP_ADD(i_, j_)
        , F
        AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, T)
        >
{
    typedef apply_wrap_impl0_bcb< F, aux::has_apply< F >::value >::type type;
};
#else

template<
      typename F AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(apply_wrap_impl,i_)<
          AUTOBOOST_MPL_PP_ADD(i_, j_)
        , F
        AUTOBOOST_PP_COMMA_IF(i_) AUX778076_APPLY_WRAP_PARAMS(i_, T)
        >
{
    typedef typename F::template apply<
          AUX778076_APPLY_WRAP_PARAMS(i_, T)
#if i_ == 0 && j_ == 0
/// since the defaults are "lost", we have to pass *something* even for nullary
/// metafunction classes
        na
#else
        AUTOBOOST_PP_COMMA_IF(AUTOBOOST_PP_AND(i_, j_)) AUTOBOOST_MPL_PP_ENUM(j_, na)
#endif
        > type;
};

#endif

#   undef j_

#endif // AUTOBOOST_PP_ITERATION_DEPTH()
#endif // AUTOBOOST_PP_IS_ITERATING
