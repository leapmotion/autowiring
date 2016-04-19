
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_PARTIAL_SPEC_PARAMS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_PARTIAL_SPEC_PARAMS_HPP_INCLUDED

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

#include <autoboost/mpl/limits/arity.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#include <autoboost/mpl/aux_/preprocessor/sub.hpp>
#include <autoboost/preprocessor/comma_if.hpp>

#define AUTOBOOST_MPL_PP_PARTIAL_SPEC_PARAMS(n, param, def) \
AUTOBOOST_MPL_PP_PARAMS(n, param) \
AUTOBOOST_PP_COMMA_IF(AUTOBOOST_MPL_PP_SUB(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY,n)) \
AUTOBOOST_MPL_PP_ENUM( \
      AUTOBOOST_MPL_PP_SUB(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY,n) \
    , def \
    ) \
/**/

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_PARTIAL_SPEC_PARAMS_HPP_INCLUDED
