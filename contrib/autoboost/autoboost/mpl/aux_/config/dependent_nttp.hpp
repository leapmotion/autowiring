
#ifndef AUTOBOOST_MPL_AUX_CONFIG_DEPENDENT_NTTP_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_DEPENDENT_NTTP_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// GCC and EDG-based compilers incorrectly reject the following code:
//   template< typename T, T n > struct a;
//   template< typename T > struct b;
//   template< typename T, T n > struct b< a<T,n> > {};

#if    !defined(AUTOBOOST_MPL_CFG_NO_DEPENDENT_NONTYPE_PARAMETER_IN_PARTIAL_SPEC) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && (   AUTOBOOST_WORKAROUND(__EDG_VERSION__, AUTOBOOST_TESTED_AT(300)) \
        || AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0302)) \
        )

#   define AUTOBOOST_MPL_CFG_NO_DEPENDENT_NONTYPE_PARAMETER_IN_PARTIAL_SPEC

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_DEPENDENT_NTTP_HPP_INCLUDED
