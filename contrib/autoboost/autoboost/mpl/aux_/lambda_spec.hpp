
#ifndef AUTOBOOST_MPL_AUX_LAMBDA_SPEC_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_LAMBDA_SPEC_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2007
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/lambda_fwd.hpp>
#include <autoboost/mpl/int_fwd.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/lambda_arity_param.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   define AUTOBOOST_MPL_AUX_PASS_THROUGH_LAMBDA_SPEC(i, name) \
template< \
      AUTOBOOST_MPL_PP_PARAMS(i, typename T) \
    , typename Tag \
    > \
struct lambda< \
      name< AUTOBOOST_MPL_PP_PARAMS(i, T) > \
    , Tag \
    AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM(int_<i>) \
    > \
{ \
    typedef false_ is_le; \
    typedef name< AUTOBOOST_MPL_PP_PARAMS(i, T) > result_; \
    typedef result_ type; \
}; \
/**/

#else

#   define AUTOBOOST_MPL_AUX_PASS_THROUGH_LAMBDA_SPEC(i, name) /**/

#endif

#endif // AUTOBOOST_MPL_AUX_LAMBDA_SPEC_HPP_INCLUDED
