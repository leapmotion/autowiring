
#ifndef AUTOBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED

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

#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/limits/arity.hpp>
#include <autoboost/mpl/aux_/config/dtp.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/arity.hpp>
#include <autoboost/mpl/aux_/template_arity_fwd.hpp>
#include <autoboost/mpl/aux_/config/ttp.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>

#if defined(AUTOBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define AUTOBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,type,name) \
namespace aux { \
template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, N), AUTOBOOST_MPL_PP_PARAMS(i,type T) > \
struct arity< \
      name< AUTOBOOST_MPL_PP_PARAMS(i,T) > \
    , N \
    > \
{ \
    AUTOBOOST_STATIC_CONSTANT(int \
        , value = AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        ); \
}; \
} \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,type,name) /**/
#endif

#   define AUTOBOOST_MPL_AUX_ARITY_SPEC(i,name) \
    AUTOBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,typename,name) \
/**/


#if defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING) \
    && !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   define AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(i, name) \
namespace aux { \
template< AUTOBOOST_MPL_PP_PARAMS(i,typename T) > \
struct template_arity< name<AUTOBOOST_MPL_PP_PARAMS(i,T)> > \
    : int_<i> \
{ \
}; \
} \
/**/
#else
#   define AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(i, name) /**/
#endif


#endif // AUTOBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED
