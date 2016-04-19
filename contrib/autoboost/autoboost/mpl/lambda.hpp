
#ifndef AUTOBOOST_MPL_LAMBDA_HPP_INCLUDED
#define AUTOBOOST_MPL_LAMBDA_HPP_INCLUDED

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

#include <autoboost/mpl/lambda_fwd.hpp>
#include <autoboost/mpl/bind.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   include <autoboost/mpl/aux_/full_lambda.hpp>
#else
#   include <autoboost/mpl/aux_/lambda_no_ctps.hpp>
#   include <autoboost/mpl/aux_/lambda_support.hpp>
#   define AUTOBOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS
#endif

#endif // AUTOBOOST_MPL_LAMBDA_HPP_INCLUDED
