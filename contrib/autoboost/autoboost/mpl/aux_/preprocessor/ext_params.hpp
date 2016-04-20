
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_EXT_PARAMS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_EXT_PARAMS_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/preprocessor.hpp>

// AUTOBOOST_MPL_PP_EXT_PARAMS(2,2,T): <nothing>
// AUTOBOOST_MPL_PP_EXT_PARAMS(2,3,T): T2
// AUTOBOOST_MPL_PP_EXT_PARAMS(2,4,T): T2, T3
// AUTOBOOST_MPL_PP_EXT_PARAMS(2,n,T): T2, T3, .., Tn-1

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/mpl/aux_/preprocessor/filter_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/sub.hpp>

#   define AUTOBOOST_MPL_PP_EXT_PARAMS(i,j,p) \
    AUTOBOOST_MPL_PP_EXT_PARAMS_DELAY_1(i,AUTOBOOST_MPL_PP_SUB(j,i),p) \
    /**/

#   define AUTOBOOST_MPL_PP_EXT_PARAMS_DELAY_1(i,n,p) \
    AUTOBOOST_MPL_PP_EXT_PARAMS_DELAY_2(i,n,p) \
    /**/

#   define AUTOBOOST_MPL_PP_EXT_PARAMS_DELAY_2(i,n,p) \
    AUTOBOOST_MPL_PP_EXT_PARAMS_##i(n,p) \
    /**/

#   define AUTOBOOST_MPL_PP_EXT_PARAMS_1(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##1,p##2,p##3,p##4,p##5,p##6,p##7,p##8,p##9)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_2(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##2,p##3,p##4,p##5,p##6,p##7,p##8,p##9,p1)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_3(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##3,p##4,p##5,p##6,p##7,p##8,p##9,p1,p2)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_4(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##4,p##5,p##6,p##7,p##8,p##9,p1,p2,p3)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_5(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##5,p##6,p##7,p##8,p##9,p1,p2,p3,p4)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_6(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##6,p##7,p##8,p##9,p1,p2,p3,p4,p5)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_7(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##7,p##8,p##9,p1,p2,p3,p4,p5,p6)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_8(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##8,p##9,p1,p2,p3,p4,p5,p6,p7)
#   define AUTOBOOST_MPL_PP_EXT_PARAMS_9(i,p) AUTOBOOST_MPL_PP_FILTER_PARAMS_##i(p##9,p1,p2,p3,p4,p5,p6,p7,p8)

#else

#   include <autoboost/preprocessor/arithmetic/add.hpp>
#   include <autoboost/preprocessor/arithmetic/sub.hpp>
#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/tuple/elem.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_AUX_EXT_PARAM_FUNC(unused, i, op) \
    AUTOBOOST_PP_COMMA_IF(i) \
    AUTOBOOST_PP_CAT( \
          AUTOBOOST_PP_TUPLE_ELEM(2,1,op) \
        , AUTOBOOST_PP_ADD_D(1, i, AUTOBOOST_PP_TUPLE_ELEM(2,0,op)) \
        ) \
    /**/

#   define AUTOBOOST_MPL_PP_EXT_PARAMS(i, j, param) \
    AUTOBOOST_PP_REPEAT( \
          AUTOBOOST_PP_SUB_D(1,j,i) \
        , AUTOBOOST_MPL_PP_AUX_EXT_PARAM_FUNC \
        , (i,param) \
        ) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_EXT_PARAMS_HPP_INCLUDED
