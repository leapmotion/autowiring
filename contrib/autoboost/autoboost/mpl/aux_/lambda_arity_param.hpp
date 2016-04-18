
#ifndef AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/ttp.hpp>

#if !defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING)
#   define AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM(param)
#else
#   define AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM(param) , param
#endif

#endif // AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM_HPP_INCLUDED
