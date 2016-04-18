
#ifndef AUTOBOOST_MPL_LAMBDA_FWD_HPP_INCLUDED
#define AUTOBOOST_MPL_LAMBDA_FWD_HPP_INCLUDED

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

#include <autoboost/mpl/void_fwd.hpp>
#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   include <autoboost/mpl/int.hpp>
#   include <autoboost/mpl/aux_/lambda_arity_param.hpp>
#   include <autoboost/mpl/aux_/template_arity_fwd.hpp>

namespace autoboost { namespace mpl {

template<
      typename T = na
    , typename Tag = void_
    AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM(
          typename Arity = int_< aux::template_arity<T>::value >
        )
    >
struct lambda;

}}

#else // AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#   include <autoboost/mpl/bool.hpp>

namespace autoboost { namespace mpl {

template<
      typename T = na
    , typename Tag = void_
    , typename Protect = true_
    >
struct lambda;

}}

#endif

#endif // AUTOBOOST_MPL_LAMBDA_FWD_HPP_INCLUDED
