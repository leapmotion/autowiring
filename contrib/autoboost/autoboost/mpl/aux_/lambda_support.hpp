
#ifndef AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) /**/
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i,name,params) /**/

#else

#   include <autoboost/mpl/int_fwd.hpp>
#   include <autoboost/mpl/aux_/yes_no.hpp>
#   include <autoboost/mpl/aux_/na_fwd.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#   include <autoboost/mpl/aux_/config/msvc.hpp>
#   include <autoboost/mpl/aux_/config/workaround.hpp>

#   include <autoboost/preprocessor/tuple/to_list.hpp>
#   include <autoboost/preprocessor/list/for_each_i.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC(R,typedef_,i,param) \
    typedef_ param AUTOBOOST_PP_CAT(arg,AUTOBOOST_PP_INC(i)); \
    /**/

// agurt, 07/mar/03: restore an old revision for the sake of SGI MIPSpro C++
#if AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 238)

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    typedef AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    AUTOBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , AUTOBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind \
    { \
        template< AUTOBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
            : name< AUTOBOOST_MPL_PP_PARAMS(i,U) > \
        { \
        }; \
    }; \
    /**/

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    /**/

#elif AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(AUTOBOOST_INTEL_CXX_VERSION)
// agurt, 18/jan/03: old EDG-based compilers actually enforce 11.4 para 9
// (in strict mode), so we have to provide an alternative to the
// MSVC-optimized implementation

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    AUTOBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , AUTOBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind; \
/**/

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
template< AUTOBOOST_MPL_PP_PARAMS(i,typename T) > \
struct name<AUTOBOOST_MPL_PP_PARAMS(i,T)>::rebind \
{ \
    template< AUTOBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< AUTOBOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/

#else // __EDG_VERSION__

namespace autoboost { namespace mpl { namespace aux {
template< typename T > struct has_rebind_tag;
}}}

#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    AUTOBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , AUTOBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    friend class AUTOBOOST_PP_CAT(name,_rebind); \
    typedef AUTOBOOST_PP_CAT(name,_rebind) rebind; \
/**/

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< AUTOBOOST_MPL_PP_PARAMS(i,typename T) > \
::autoboost::mpl::aux::yes_tag operator|( \
      ::autoboost::mpl::aux::has_rebind_tag<int> \
    , name<AUTOBOOST_MPL_PP_PARAMS(i,T)>* \
    ); \
::autoboost::mpl::aux::no_tag operator|( \
      ::autoboost::mpl::aux::has_rebind_tag<int> \
    , name< AUTOBOOST_MPL_PP_ENUM(i,::autoboost::mpl::na) >* \
    ); \
/**/
#elif !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< AUTOBOOST_MPL_PP_PARAMS(i,typename T) > \
::autoboost::mpl::aux::yes_tag operator|( \
      ::autoboost::mpl::aux::has_rebind_tag<int> \
    , ::autoboost::mpl::aux::has_rebind_tag< name<AUTOBOOST_MPL_PP_PARAMS(i,T)> >* \
    ); \
/**/
#else
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) /**/
#endif

#   if !defined(__BORLANDC__)
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class AUTOBOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< AUTOBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< AUTOBOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/
#   else
#   define AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class AUTOBOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< AUTOBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
    { \
        typedef typename name< AUTOBOOST_MPL_PP_PARAMS(i,U) >::type type; \
    }; \
/**/
#   endif // __BORLANDC__

#endif // __EDG_VERSION__

#endif // AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#endif // AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
