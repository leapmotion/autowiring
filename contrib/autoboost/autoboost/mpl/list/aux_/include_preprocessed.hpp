
// Copyright Aleksey Gurtovoy 2001-2006
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION!

#include <autoboost/mpl/aux_/config/workaround.hpp>

#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/stringize.hpp>

#   define AUX778076_HEADER \
    aux_/preprocessed/plain/AUTOBOOST_MPL_PREPROCESSED_HEADER \
/**/

#if AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(700))
#   define AUX778076_INCLUDE_STRING AUTOBOOST_PP_STRINGIZE(autoboost/mpl/list/AUX778076_HEADER)
#   include AUX778076_INCLUDE_STRING
#   undef AUX778076_INCLUDE_STRING
#else
#   include AUTOBOOST_PP_STRINGIZE(autoboost/mpl/list/AUX778076_HEADER)
#endif

#   undef AUX778076_HEADER

#undef AUTOBOOST_MPL_PREPROCESSED_HEADER
