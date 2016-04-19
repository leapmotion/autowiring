
#ifndef AUTOBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !defined(AUTOBOOST_MPL_CFG_BROKEN_PP_MACRO_EXPANSION) \
    && (   AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
        || AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x582) \
        || AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(502)) \
        )

#   define AUTOBOOST_MPL_CFG_BROKEN_PP_MACRO_EXPANSION

#endif

#if !defined(AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)
#   define AUTOBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES
#endif

#if !defined(AUTOBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING) \
    && AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
#   define AUTOBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING
#endif


#endif // AUTOBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED
