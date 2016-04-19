
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED

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

#include <autoboost/mpl/limits/arity.hpp>
#include <autoboost/mpl/aux_/config/dtp.hpp>
#include <autoboost/mpl/aux_/config/preprocessor.hpp>

#include <autoboost/preprocessor/comma_if.hpp>
#include <autoboost/preprocessor/logical/and.hpp>
#include <autoboost/preprocessor/identity.hpp>
#include <autoboost/preprocessor/empty.hpp>

// AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL(1,T,value): , T1 = value, .., Tn = value
// AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL(2,T,value): , T2 = value, .., Tn = value
// AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL(n,T,value): <nothing>

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/mpl/aux_/preprocessor/filter_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/sub.hpp>

#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, value_func) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_1( \
          i \
        , AUTOBOOST_MPL_PP_SUB(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY,i) \
        , param \
        , value_func \
        ) \
    /**/

#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_1(i, n, param, value_func) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_2(i,n,param,value_func) \
    /**/

#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_2(i, n, param, value_func) \
    AUTOBOOST_PP_COMMA_IF(AUTOBOOST_PP_AND(i,n)) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_##i(n,param,value_func) \
    /**/

#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_0(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##1 v(),p##2 v(),p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v())
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_1(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##2 v(),p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_2(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_3(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_4(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_5(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4,p5)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_6(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4,p5,p6)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_7(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##8 v(),p##9 v(),p1,p2,p3,p4,p5,p6,p7)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_8(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##9 v(),p1,p2,p3,p4,p5,p6,p7,p8)
#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_9(i,p,v) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p1,p2,p3,p4,p5,p6,p7,p8,p9)

#else

#   include <autoboost/preprocessor/arithmetic/add.hpp>
#   include <autoboost/preprocessor/arithmetic/sub.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/tuple/elem.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_AUX_TAIL_PARAM_FUNC(unused, i, op) \
    , AUTOBOOST_PP_CAT( \
          AUTOBOOST_PP_TUPLE_ELEM(3, 1, op) \
        , AUTOBOOST_PP_ADD_D(1, i, AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(3, 0, op))) \
        ) AUTOBOOST_PP_TUPLE_ELEM(3, 2, op)() \
    /**/

#   define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, value_func) \
    AUTOBOOST_PP_REPEAT( \
          AUTOBOOST_PP_SUB_D(1, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, i) \
        , AUTOBOOST_MPL_PP_AUX_TAIL_PARAM_FUNC \
        , (i, param, value_func) \
        ) \
    /**/


#endif // AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES

#define AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL(i, param, value) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, AUTOBOOST_PP_IDENTITY(=value)) \
    /**/

#if !defined(AUTOBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define AUTOBOOST_MPL_PP_NESTED_DEF_PARAMS_TAIL(i, param, value) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, AUTOBOOST_PP_IDENTITY(=value)) \
    /**/
#else
#   define AUTOBOOST_MPL_PP_NESTED_DEF_PARAMS_TAIL(i, param, value) \
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, AUTOBOOST_PP_EMPTY) \
    /**/
#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED
