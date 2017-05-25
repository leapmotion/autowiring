
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_TOKEN_EQUAL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_TOKEN_EQUAL_HPP_INCLUDED

// Copyright Paul Mensonides 2003
// Copyright Aleksey Gurtovoy 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/preprocessor/is_seq.hpp>

#include <autoboost/preprocessor/if.hpp>
#include <autoboost/preprocessor/logical/bitand.hpp>
#include <autoboost/preprocessor/logical/compl.hpp>
#include <autoboost/preprocessor/tuple/eat.hpp>
#include <autoboost/preprocessor/cat.hpp>

// compares tokens 'a' and 'b' for equality:
//
//   #define AUTOBOOST_MPL_PP_TOKEN_EQUAL_apple(x) x
//   #define AUTOBOOST_MPL_PP_TOKEN_EQUAL_orange(x) x
//
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_PP_NOT( AUTOBOOST_MPL_PP_TOKEN_EQUAL(apple, abc) ) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_PP_NOT( AUTOBOOST_MPL_PP_TOKEN_EQUAL(abc, apple) ) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_PP_NOT( AUTOBOOST_MPL_PP_TOKEN_EQUAL(apple, orange) ) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_MPL_PP_TOKEN_EQUAL(apple, apple) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_MPL_PP_TOKEN_EQUAL(orange, orange) )

#define AUTOBOOST_MPL_PP_TOKEN_EQUAL(a, b) \
    AUTOBOOST_PP_IIF( \
        AUTOBOOST_PP_BITAND( \
              AUTOBOOST_MPL_PP_IS_SEQ( AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_TOKEN_EQUAL_, a)((unused)) ) \
            , AUTOBOOST_MPL_PP_IS_SEQ( AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_TOKEN_EQUAL_, b)((unused)) ) \
            ) \
        , AUTOBOOST_MPL_PP_TOKEN_EQUAL_I \
        , 0 AUTOBOOST_PP_TUPLE_EAT(2) \
        )(a, b) \
/**/

#define AUTOBOOST_MPL_PP_TOKEN_EQUAL_I(a, b) \
    AUTOBOOST_PP_COMPL(AUTOBOOST_MPL_PP_IS_SEQ( \
        AUTOBOOST_MPL_PP_TOKEN_EQUAL_ ## a( \
            AUTOBOOST_MPL_PP_TOKEN_EQUAL_ ## b \
            )((unused)) \
        )) \
/**/

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_TOKEN_EQUAL_HPP_INCLUDED
