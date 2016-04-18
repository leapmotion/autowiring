
#ifndef AUTOBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED

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
#include <autoboost/mpl/aux_/config/intel.hpp>
#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// agurt, 25/apr/04: technically, the ADL workaround is only needed for GCC,
// but putting everything expect public, user-specializable metafunctions into
// a separate global namespace has a nice side effect of reducing the length
// of template instantiation symbols, so we apply the workaround on all
// platforms that can handle it

#if !defined(AUTOBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE) \
    && (   AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1400)) \
        || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) \
        || AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840)) \
        || AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3202)) \
        || AUTOBOOST_WORKAROUND(AUTOBOOST_INTEL_CXX_VERSION, AUTOBOOST_TESTED_AT(810)) \
        )

#   define AUTOBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED
