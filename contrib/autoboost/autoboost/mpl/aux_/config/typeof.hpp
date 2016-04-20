
#ifndef AUTOBOOST_MPL_AUX_CONFIG_TYPEOF_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_TYPEOF_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_CFG_HAS_TYPEOF) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && (   defined(AUTOBOOST_MPL_CFG_GCC) && AUTOBOOST_MPL_CFG_GCC >= 0x0302 \
        || defined(__MWERKS__) && __MWERKS__ >= 0x3000 \
        )

#   define AUTOBOOST_MPL_CFG_HAS_TYPEOF

#endif


#if !defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && defined(AUTOBOOST_MPL_CFG_HAS_TYPEOF)

#   define AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_TYPEOF_HPP_INCLUDED
