
#ifndef AUTOBOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if    !defined(AUTOBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))

#   define AUTOBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS

#endif

#if    !defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && ( AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
        || AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 238) \
        )

#   define AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED
