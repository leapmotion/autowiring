
#ifndef AUTOBOOST_MPL_AUX_CONFIG_OPERATORS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_OPERATORS_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !defined(AUTOBOOST_MPL_CFG_USE_OPERATORS_OVERLOADING) \
    && ( AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
        || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) \
        || AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 245) \
        || AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, <= 0x0295) \
        || AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(600)) \
        || AUTOBOOST_WORKAROUND(__NVCC__, AUTOBOOST_TESTED_AT(1)) \
        )

#   define AUTOBOOST_MPL_CFG_USE_OPERATORS_OVERLOADING

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_OPERATORS_HPP_INCLUDED
