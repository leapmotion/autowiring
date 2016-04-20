
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_ADD_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_ADD_HPP_INCLUDED

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

#   define AUTOBOOST_MPL_PP_ADD(i,j) \
    AUTOBOOST_MPL_PP_ADD_DELAY(i,j) \
    /**/

#   define AUTOBOOST_MPL_PP_ADD_DELAY(i,j) \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_TUPLE_11_ELEM_##i,AUTOBOOST_MPL_PP_ADD_##j) \
    /**/
#else
#   define AUTOBOOST_MPL_PP_ADD(i,j) \
    AUTOBOOST_MPL_PP_ADD_DELAY(i,j) \
    /**/

#   define AUTOBOOST_MPL_PP_ADD_DELAY(i,j) \
    AUTOBOOST_MPL_PP_TUPLE_11_ELEM_##i AUTOBOOST_MPL_PP_ADD_##j \
    /**/
#endif

#   define AUTOBOOST_MPL_PP_ADD_0 (0,1,2,3,4,5,6,7,8,9,10)
#   define AUTOBOOST_MPL_PP_ADD_1 (1,2,3,4,5,6,7,8,9,10,0)
#   define AUTOBOOST_MPL_PP_ADD_2 (2,3,4,5,6,7,8,9,10,0,0)
#   define AUTOBOOST_MPL_PP_ADD_3 (3,4,5,6,7,8,9,10,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_4 (4,5,6,7,8,9,10,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_5 (5,6,7,8,9,10,0,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_6 (6,7,8,9,10,0,0,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_7 (7,8,9,10,0,0,0,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_8 (8,9,10,0,0,0,0,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_9 (9,10,0,0,0,0,0,0,0,0,0)
#   define AUTOBOOST_MPL_PP_ADD_10 (10,0,0,0,0,0,0,0,0,0,0)

#else

#   include <autoboost/preprocessor/arithmetic/add.hpp>

#   define AUTOBOOST_MPL_PP_ADD(i,j) \
    AUTOBOOST_PP_ADD(i,j) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_ADD_HPP_INCLUDED
