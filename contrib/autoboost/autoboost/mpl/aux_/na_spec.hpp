
#ifndef AUTOBOOST_MPL_AUX_NA_SPEC_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_NA_SPEC_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/lambda_fwd.hpp>
#   include <autoboost/mpl/int.hpp>
#   include <autoboost/mpl/bool.hpp>
#   include <autoboost/mpl/aux_/na.hpp>
#   include <autoboost/mpl/aux_/arity.hpp>
#   include <autoboost/mpl/aux_/template_arity_fwd.hpp>
#endif

#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#include <autoboost/mpl/aux_/preprocessor/def_params_tail.hpp>
#include <autoboost/mpl/aux_/lambda_arity_param.hpp>
#include <autoboost/mpl/aux_/config/dtp.hpp>
#include <autoboost/mpl/aux_/config/eti.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/config/ttp.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>
#include <autoboost/mpl/aux_/config/overload_resolution.hpp>


#define AUTOBOOST_MPL_AUX_NA_PARAMS(i) \
    AUTOBOOST_MPL_PP_ENUM(i, na) \
/**/

#if defined(AUTOBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define AUTOBOOST_MPL_AUX_NA_SPEC_ARITY(i, name) \
namespace aux { \
template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, N) > \
struct arity< \
          name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > \
        , N \
        > \
    : int_< AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY > \
{ \
}; \
} \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NA_SPEC_ARITY(i, name) /**/
#endif

#define AUTOBOOST_MPL_AUX_NA_SPEC_MAIN(i, name) \
template<> \
struct name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > \
{ \
    template< \
          AUTOBOOST_MPL_PP_PARAMS(i, typename T) \
        AUTOBOOST_MPL_PP_NESTED_DEF_PARAMS_TAIL(i, typename T, na) \
        > \
    struct apply \
        : name< AUTOBOOST_MPL_PP_PARAMS(i, T) > \
    { \
    }; \
}; \
/**/

#if defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   define AUTOBOOST_MPL_AUX_NA_SPEC_LAMBDA(i, name) \
template<> \
struct lambda< \
      name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > \
    , void_ \
    , true_ \
    > \
{ \
    typedef false_ is_le; \
    typedef name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > type; \
}; \
template<> \
struct lambda< \
      name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > \
    , void_ \
    , false_ \
    > \
{ \
    typedef false_ is_le; \
    typedef name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > type; \
}; \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NA_SPEC_LAMBDA(i, name) \
template< typename Tag > \
struct lambda< \
      name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > \
    , Tag \
    AUTOBOOST_MPL_AUX_LAMBDA_ARITY_PARAM(int_<-1>) \
    > \
{ \
    typedef false_ is_le; \
    typedef name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > result_; \
    typedef name< AUTOBOOST_MPL_AUX_NA_PARAMS(i) > type; \
}; \
/**/
#endif

#if defined(AUTOBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING) \
    || defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT) \
        && defined(AUTOBOOST_MPL_CFG_BROKEN_OVERLOAD_RESOLUTION)
#   define AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(i, j, name) \
namespace aux { \
template< AUTOBOOST_MPL_PP_PARAMS(j, typename T) > \
struct template_arity< \
          name< AUTOBOOST_MPL_PP_PARAMS(j, T) > \
        > \
    : int_<j> \
{ \
}; \
\
template<> \
struct template_arity< \
          name< AUTOBOOST_MPL_PP_ENUM(i, na) > \
        > \
    : int_<-1> \
{ \
}; \
} \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(i, j, name) /**/
#endif

#if defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)
#   define AUTOBOOST_MPL_AUX_NA_SPEC_ETI(i, name) \
template<> \
struct name< AUTOBOOST_MPL_PP_ENUM(i, int) > \
{ \
    typedef int type; \
    enum { value = 0 }; \
}; \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NA_SPEC_ETI(i, name) /**/
#endif

#define AUTOBOOST_MPL_AUX_NA_PARAM(param) param = na

#define AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_MAIN(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_LAMBDA(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_ARITY(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(i, i, name) \
/**/

#define AUTOBOOST_MPL_AUX_NA_SPEC(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_ETI(i, name) \
/**/

#define AUTOBOOST_MPL_AUX_NA_SPEC2(i, j, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_MAIN(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_ETI(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_LAMBDA(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_ARITY(i, name) \
AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(i, j, name) \
/**/


#endif // AUTOBOOST_MPL_AUX_NA_SPEC_HPP_INCLUDED
