
#ifndef AUTOBOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
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
#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
    && ( defined(AUTOBOOST_NO_TEMPLATE_TEMPLATES) \
      || AUTOBOOST_WORKAROUND( __BORLANDC__, AUTOBOOST_TESTED_AT( 0x590) ) \
       )

#   define AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS

#endif


#if    !defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && (   AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0302)) \
        || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) \
        )

#   define AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED
