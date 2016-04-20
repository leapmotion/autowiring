
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2000-2006
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/compiler.hpp>
#include <autoboost/mpl/aux_/config/preprocessor.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/stringize.hpp>

#if !defined(AUTOBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING)
#   define AUX778076_PREPROCESSED_HEADER \
    AUTOBOOST_MPL_CFG_COMPILER_DIR/AUTOBOOST_MPL_PREPROCESSED_HEADER \
/**/
#else
#   define AUX778076_PREPROCESSED_HEADER \
    AUTOBOOST_PP_CAT(AUTOBOOST_MPL_CFG_COMPILER_DIR,/)##AUTOBOOST_MPL_PREPROCESSED_HEADER \
/**/
#endif

#if AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(700))
#   define AUX778076_INCLUDE_STRING AUTOBOOST_PP_STRINGIZE(autoboost/mpl/aux_/preprocessed/AUX778076_PREPROCESSED_HEADER)
#   include AUX778076_INCLUDE_STRING
#   undef AUX778076_INCLUDE_STRING
#else
#   include AUTOBOOST_PP_STRINGIZE(autoboost/mpl/aux_/preprocessed/AUX778076_PREPROCESSED_HEADER)
#endif

#   undef AUX778076_PREPROCESSED_HEADER

#undef AUTOBOOST_MPL_PREPROCESSED_HEADER
