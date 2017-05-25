
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_IS_SEQ_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_IS_SEQ_HPP_INCLUDED

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

#include <autoboost/preprocessor/seq/size.hpp>
#include <autoboost/preprocessor/arithmetic/dec.hpp>
#include <autoboost/preprocessor/punctuation/paren.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/config/config.hpp>

// returns 1 if 'seq' is a PP-sequence, 0 otherwise:
//
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_PP_NOT( AUTOBOOST_MPL_PP_IS_SEQ( int ) ) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_MPL_PP_IS_SEQ( (int) ) )
//   AUTOBOOST_PP_ASSERT( AUTOBOOST_MPL_PP_IS_SEQ( (1)(2) ) )

#if (AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_BCC()) || defined(_MSC_VER) && defined(__INTEL_COMPILER) && __INTEL_COMPILER == 1010

#   define AUTOBOOST_MPL_PP_IS_SEQ(seq) AUTOBOOST_PP_DEC( AUTOBOOST_PP_SEQ_SIZE( AUTOBOOST_MPL_PP_IS_SEQ_(seq) ) )
#   define AUTOBOOST_MPL_PP_IS_SEQ_(seq) AUTOBOOST_MPL_PP_IS_SEQ_SEQ_( AUTOBOOST_MPL_PP_IS_SEQ_SPLIT_ seq )
#   define AUTOBOOST_MPL_PP_IS_SEQ_SEQ_(x) (x)
#   define AUTOBOOST_MPL_PP_IS_SEQ_SPLIT_(unused) unused)((unused)

#else

#   if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#       define AUTOBOOST_MPL_PP_IS_SEQ(seq) AUTOBOOST_MPL_PP_IS_SEQ_MWCC_((seq))
#       define AUTOBOOST_MPL_PP_IS_SEQ_MWCC_(args) AUTOBOOST_MPL_PP_IS_SEQ_ ## args
#   else
#       define AUTOBOOST_MPL_PP_IS_SEQ(seq) AUTOBOOST_MPL_PP_IS_SEQ_(seq)
#   endif

#   define AUTOBOOST_MPL_PP_IS_SEQ_(seq) AUTOBOOST_PP_CAT(AUTOBOOST_MPL_PP_IS_SEQ_, AUTOBOOST_MPL_PP_IS_SEQ_0 seq AUTOBOOST_PP_RPAREN())
#   define AUTOBOOST_MPL_PP_IS_SEQ_0(x) AUTOBOOST_MPL_PP_IS_SEQ_1(x
#   define AUTOBOOST_MPL_PP_IS_SEQ_ALWAYS_0(unused) 0
#   define AUTOBOOST_MPL_PP_IS_SEQ_AUTOBOOST_MPL_PP_IS_SEQ_0 AUTOBOOST_MPL_PP_IS_SEQ_ALWAYS_0(
#   define AUTOBOOST_MPL_PP_IS_SEQ_AUTOBOOST_MPL_PP_IS_SEQ_1(unused) 1

#endif

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_IS_SEQ_HPP_INCLUDED
