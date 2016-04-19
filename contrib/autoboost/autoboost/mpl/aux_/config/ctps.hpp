
#ifndef AUTOBOOST_MPL_AUX_CONFIG_CTPS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_CTPS_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/config.hpp>

#if    !defined(AUTOBOOST_MPL_CFG_NO_NONTYPE_TEMPLATE_PARTIAL_SPEC) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x582)

#   define AUTOBOOST_MPL_CFG_NO_NONTYPE_TEMPLATE_PARTIAL_SPEC

#endif

// AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION is defined in <autoboost/config.hpp>

#endif // AUTOBOOST_MPL_AUX_CONFIG_CTPS_HPP_INCLUDED
