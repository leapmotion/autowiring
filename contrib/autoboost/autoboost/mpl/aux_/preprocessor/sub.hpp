
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_SUB_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_SUB_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/mpl/aux_/preprocessor/tuple.hpp>

#if defined(AUTOBOOST_MPL_CFG_BROKEN_PP_MACRO_EXPANSION)
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_SUB(i,j) \
    AUTOBOOST_MPL_PP_SUB_DELAY(i,j) \
    /**/

#   define AUTOBOOST_MPL_PP_SUB_DELAY(i,j) \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_TUPLE_11_ELEM_##i,AUTOBOOST_MPL_PP_SUB_##j) \
    /**/
#else
#   define AUTOBOOST_MPL_PP_SUB(i,j) \
    AUTOBOOST_MPL_PP_SUB_DELAY(i,j) \
    /**/

#   define AUTOBOOST_MPL_PP_SUB_DELAY(i,j) \
    AUTOBOOST_MPL_PP_TUPLE_11_ELEM_##i AUTOBOOST_MPL_PP_SUB_##j \
    /**/
#endif

#   define AUTOBOOST_MPL_PP_SUB_0 (0,1,2,3,4,5,6,7,8,9,10)
#   define AUTOBOOST_MPL_PP_SUB_1 (0,0,1,2,3,4,5,6,7,8,9)
#   define AUTOBOOST_MPL_PP_SUB_2 (0,0,0,1,2,3,4,5,6,7,8)
#   define AUTOBOOST_MPL_PP_SUB_3 (0,0,0,0,1,2,3,4,5,6,7)
#   define AUTOBOOST_MPL_PP_SUB_4 (0,0,0,0,0,1,2,3,4,5,6)
#   define AUTOBOOST_MPL_PP_SUB_5 (0,0,0,0,0,0,1,2,3,4,5)
#   define AUTOBOOST_MPL_PP_SUB_6 (0,0,0,0,0,0,0,1,2,3,4)
#   define AUTOBOOST_MPL_PP_SUB_7 (0,0,0,0,0,0,0,0,1,2,3)
#   define AUTOBOOST_MPL_PP_SUB_8 (0,0,0,0,0,0,0,0,0,1,2)
#   define AUTOBOOST_MPL_PP_SUB_9 (0,0,0,0,0,0,0,0,0,0,1)
#   define AUTOBOOST_MPL_PP_SUB_10 (0,0,0,0,0,0,0,0,0,0,0)

#else

#   include <autoboost/preprocessor/arithmetic/sub.hpp>

#   define AUTOBOOST_MPL_PP_SUB(i,j) \
    AUTOBOOST_PP_SUB(i,j) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_SUB_HPP_INCLUDED
