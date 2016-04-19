
#ifndef AUTOBOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED

// Copyright David Abrahams 2002
// Copyright Aleksey Gurtovoy 2002-2004
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

#if    !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && (   AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
        || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) \
        )

#   define AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE

#endif

//#define AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT

#endif // AUTOBOOST_MPL_AUX_CONFIG_BIND_HPP_INCLUDED
