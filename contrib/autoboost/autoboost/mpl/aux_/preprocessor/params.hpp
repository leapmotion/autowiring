
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_PARAMS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_PARAMS_HPP_INCLUDED

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

// AUTOBOOST_MPL_PP_PARAMS(0,T): <nothing>
// AUTOBOOST_MPL_PP_PARAMS(1,T): T1
// AUTOBOOST_MPL_PP_PARAMS(2,T): T1, T2
// AUTOBOOST_MPL_PP_PARAMS(n,T): T1, T2, .., Tn

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_PARAMS(n,p) \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_PARAMS_,n)(p) \
    /**/

#   define AUTOBOOST_MPL_PP_PARAMS_0(p)
#   define AUTOBOOST_MPL_PP_PARAMS_1(p) p##1
#   define AUTOBOOST_MPL_PP_PARAMS_2(p) p##1,p##2
#   define AUTOBOOST_MPL_PP_PARAMS_3(p) p##1,p##2,p##3
#   define AUTOBOOST_MPL_PP_PARAMS_4(p) p##1,p##2,p##3,p##4
#   define AUTOBOOST_MPL_PP_PARAMS_5(p) p##1,p##2,p##3,p##4,p##5
#   define AUTOBOOST_MPL_PP_PARAMS_6(p) p##1,p##2,p##3,p##4,p##5,p##6
#   define AUTOBOOST_MPL_PP_PARAMS_7(p) p##1,p##2,p##3,p##4,p##5,p##6,p##7
#   define AUTOBOOST_MPL_PP_PARAMS_8(p) p##1,p##2,p##3,p##4,p##5,p##6,p##7,p##8
#   define AUTOBOOST_MPL_PP_PARAMS_9(p) p##1,p##2,p##3,p##4,p##5,p##6,p##7,p##8,p##9

#else

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_AUX_PARAM_FUNC(unused, i, param) \
    AUTOBOOST_PP_COMMA_IF(i) \
    AUTOBOOST_PP_CAT(param, AUTOBOOST_PP_INC(i)) \
    /**/

#   define AUTOBOOST_MPL_PP_PARAMS(n, param) \
    AUTOBOOST_PP_REPEAT( \
          n \
        , AUTOBOOST_MPL_PP_AUX_PARAM_FUNC \
        , param \
        ) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_PARAMS_HPP_INCLUDED
