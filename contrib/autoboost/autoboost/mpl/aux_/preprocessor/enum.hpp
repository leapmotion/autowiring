
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED

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

// AUTOBOOST_MPL_PP_ENUM(0,int): <nothing>
// AUTOBOOST_MPL_PP_ENUM(1,int): int
// AUTOBOOST_MPL_PP_ENUM(2,int): int, int
// AUTOBOOST_MPL_PP_ENUM(n,int): int, int, .., int

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_PP_ENUM(n, param) \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_ENUM_,n)(param) \
    /**/

#   define AUTOBOOST_MPL_PP_ENUM_0(p)
#   define AUTOBOOST_MPL_PP_ENUM_1(p) p
#   define AUTOBOOST_MPL_PP_ENUM_2(p) p,p
#   define AUTOBOOST_MPL_PP_ENUM_3(p) p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_4(p) p,p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_5(p) p,p,p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_6(p) p,p,p,p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_7(p) p,p,p,p,p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_8(p) p,p,p,p,p,p,p,p
#   define AUTOBOOST_MPL_PP_ENUM_9(p) p,p,p,p,p,p,p,p,p

#else

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/repeat.hpp>

#   define AUTOBOOST_MPL_PP_AUX_ENUM_FUNC(unused, i, param) \
    AUTOBOOST_PP_COMMA_IF(i) param \
    /**/

#   define AUTOBOOST_MPL_PP_ENUM(n, param) \
    AUTOBOOST_PP_REPEAT( \
          n \
        , AUTOBOOST_MPL_PP_AUX_ENUM_FUNC \
        , param \
        ) \
    /**/

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED
