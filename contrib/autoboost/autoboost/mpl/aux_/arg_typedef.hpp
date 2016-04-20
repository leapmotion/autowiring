
#ifndef AUTOBOOST_MPL_AUX_ARG_TYPEDEF_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_ARG_TYPEDEF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/lambda.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT) \
    || AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))

#   define AUTOBOOST_MPL_AUX_ARG_TYPEDEF(T, name) typedef T name;

#else

#   define AUTOBOOST_MPL_AUX_ARG_TYPEDEF(T, name) /**/

#endif

#endif // AUTOBOOST_MPL_AUX_ARG_TYPEDEF_HPP_INCLUDED
