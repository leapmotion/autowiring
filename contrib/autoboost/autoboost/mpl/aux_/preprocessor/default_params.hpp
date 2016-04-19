
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_DEFAULT_PARAMS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_DEFAULT_PARAMS_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
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

// AUTOBOOST_MPL_PP_DEFAULT_PARAMS(0,T,int): <nothing>
// AUTOBOOST_MPL_PP_DEFAULT_PARAMS(1,T,int): T1 = int
// AUTOBOOST_MPL_PP_DEFAULT_PARAMS(2,T,int): T1 = int, T2 = int
// AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n,T,int): T1 = int, T2 = int, .., Tn = int

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n,p,v) \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_DEFAULT_PARAMS_,n)(p,v) \
    /**/

#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_0(p,v)
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_1(p,v) p##1=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_2(p,v) p##1=v,p##2=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_3(p,v) p##1=v,p##2=v,p##3=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_4(p,v) p##1=v,p##2=v,p##3=v,p##4=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_5(p,v) p##1=v,p##2=v,p##3=v,p##4=v,p##5=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_6(p,v) p##1=v,p##2=v,p##3=v,p##4=v,p##5=v,p##6=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_7(p,v) p##1=v,p##2=v,p##3=v,p##4=v,p##5=v,p##6=v,p##7=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_8(p,v) p##1=v,p##2=v,p##3=v,p##4=v,p##5=v,p##6=v,p##7=v,p##8=v
#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS_9(p,v) p##1=v,p##2=v,p##3=v,p##4=v,p##5=v,p##6=v,p##7=v,p##8=v,p##9=v

#else

#   include <autoboost/preprocessor/tuple/elem.hpp>
#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_AUX_DEFAULT_PARAM_FUNC(unused, i, pv) \
    AUTOBOOST_PP_COMMA_IF(i) \
    AUTOBOOST_PP_CAT( AUTOBOOST_PP_TUPLE_ELEM(2,0,pv), AUTOBOOST_PP_INC(i) ) \
        = AUTOBOOST_PP_TUPLE_ELEM(2,1,pv) \
    /**/

#   define AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n, param, value) \
    AUTOBOOST_PP_REPEAT( \
          n \
        , AUTOBOOST_MPL_PP_AUX_DEFAULT_PARAM_FUNC \
        , (param,value) \
        ) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_DEFAULT_PARAMS_HPP_INCLUDED
