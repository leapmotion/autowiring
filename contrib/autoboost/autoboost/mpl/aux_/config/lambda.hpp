
#ifndef AUTOBOOST_MPL_AUX_CONFIG_LAMBDA_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_LAMBDA_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/ttp.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>

// agurt, 15/jan/02: full-fledged implementation requires both
// template template parameters _and_ partial specialization

#if    !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT) \
    && (   defined(AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
        || defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
        )

#   define AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_LAMBDA_HPP_INCLUDED
