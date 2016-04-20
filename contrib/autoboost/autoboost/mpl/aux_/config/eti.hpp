
#ifndef AUTOBOOST_MPL_AUX_CONFIG_ETI_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_ETI_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// flags for MSVC 6.5's so-called "early template instantiation bug"
#if    !defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)

#   define AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG

#endif

#if    !defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)

#   define AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG

#endif

#if    !defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && ( defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG) \
        || defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG) \
        )

#   define AUTOBOOST_MPL_CFG_MSVC_ETI_BUG

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_ETI_HPP_INCLUDED
