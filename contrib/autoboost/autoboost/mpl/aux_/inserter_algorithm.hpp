
#ifndef AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/back_inserter.hpp>
#include <autoboost/mpl/front_inserter.hpp>
#include <autoboost/mpl/push_back.hpp>
#include <autoboost/mpl/push_front.hpp>
#include <autoboost/mpl/back_inserter.hpp>
#include <autoboost/mpl/front_inserter.hpp>
#include <autoboost/mpl/clear.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/aux_/common_name_wknd.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>

#include <autoboost/preprocessor/arithmetic/dec.hpp>

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

#   define AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(arity, name) \
AUTOBOOST_MPL_AUX_COMMON_NAME_WKND(name) \
template< \
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct name \
    : aux::name##_impl<AUTOBOOST_MPL_PP_PARAMS(arity, P)> \
{ \
}; \
\
template< \
      AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), typename P) \
    > \
struct name< AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P),na > \
    : if_< has_push_back< typename clear<P1>::type> \
        , aux::name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::reverse_##name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
\
template< \
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct reverse_##name \
    : aux::reverse_##name##_impl<AUTOBOOST_MPL_PP_PARAMS(arity, P)> \
{ \
}; \
\
template< \
      AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), typename P) \
    > \
struct reverse_##name< AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P),na > \
    : if_< has_push_back<P1> \
        , aux::reverse_##name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
AUTOBOOST_MPL_AUX_NA_SPEC(arity, name) \
AUTOBOOST_MPL_AUX_NA_SPEC(arity, reverse_##name) \
/**/

#else

#   define AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(arity, name) \
AUTOBOOST_MPL_AUX_COMMON_NAME_WKND(name) \
template< \
      AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), typename P) \
    > \
struct def_##name##_impl \
    : if_< has_push_back<P1> \
        , aux::name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::reverse_##name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
\
template< \
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct name \
{ \
    typedef typename eval_if< \
          is_na<AUTOBOOST_PP_CAT(P, arity)> \
        , def_##name##_impl<AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P)> \
        , aux::name##_impl<AUTOBOOST_MPL_PP_PARAMS(arity, P)> \
        >::type type; \
}; \
\
template< \
      AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), typename P) \
    > \
struct def_reverse_##name##_impl \
    : if_< has_push_back<P1> \
        , aux::reverse_##name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::name##_impl< \
              AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
template< \
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct reverse_##name \
{ \
    typedef typename eval_if< \
          is_na<AUTOBOOST_PP_CAT(P, arity)> \
        , def_reverse_##name##_impl<AUTOBOOST_MPL_PP_PARAMS(AUTOBOOST_PP_DEC(arity), P)> \
        , aux::reverse_##name##_impl<AUTOBOOST_MPL_PP_PARAMS(arity, P)> \
        >::type type; \
}; \
AUTOBOOST_MPL_AUX_NA_SPEC(arity, name) \
AUTOBOOST_MPL_AUX_NA_SPEC(arity, reverse_##name) \
/**/

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

#endif // AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED
