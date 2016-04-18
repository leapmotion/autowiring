
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

#include <autoboost/mpl/aux_/config/typeof.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>
#include <autoboost/mpl/aux_/config/preprocessor.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/stringize.hpp>

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)
#   define AUX778076_INCLUDE_DIR typeof_based
#elif defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
   || defined(AUTOBOOST_MPL_CFG_NO_NONTYPE_TEMPLATE_PARTIAL_SPEC)
#   define AUX778076_INCLUDE_DIR no_ctps
#else
#   define AUX778076_INCLUDE_DIR plain
#endif

#if !defined(AUTOBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING)
#   define AUX778076_HEADER \
    AUX778076_INCLUDE_DIR/AUTOBOOST_MPL_PREPROCESSED_HEADER \
/**/
#else
#   define AUX778076_HEADER \
    AUTOBOOST_PP_CAT(AUX778076_INCLUDE_DIR,/)##AUTOBOOST_MPL_PREPROCESSED_HEADER \
/**/
#endif


#if AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(700))
#   define AUX778076_INCLUDE_STRING AUTOBOOST_PP_STRINGIZE(autoboost/mpl/vector/aux_/preprocessed/AUX778076_HEADER)
#   include AUX778076_INCLUDE_STRING
#   undef AUX778076_INCLUDE_STRING
#else
#   include AUTOBOOST_PP_STRINGIZE(autoboost/mpl/vector/aux_/preprocessed/AUX778076_HEADER)
#endif

#   undef AUX778076_HEADER
#   undef AUX778076_INCLUDE_DIR

#undef AUTOBOOST_MPL_PREPROCESSED_HEADER
