
#ifndef AUTOBOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/sequence_tag_fwd.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/config/lambda.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   define AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC_IMPL(i, trait) /**/

#elif !defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)

#   define AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC_IMPL(i, trait) \
template<> struct trait<void_> \
{ \
    template< AUTOBOOST_MPL_PP_PARAMS(i, typename T) > struct apply \
    { \
    }; \
}; \
/**/

#else

#   define AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC_IMPL(i, trait) \
template<> struct trait<void_> \
{ \
    template< AUTOBOOST_MPL_PP_PARAMS(i, typename T) > struct apply \
    { \
    }; \
}; \
template<> struct trait<int> \
{ \
    template< AUTOBOOST_MPL_PP_PARAMS(i, typename T) > struct apply \
    { \
        typedef int type; \
    }; \
}; \
/**/

#endif // AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT


#define AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(i, trait) \
    AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC_IMPL(i, trait) \
    template<> struct trait<non_sequence_tag> {}; \
/**/

#endif // AUTOBOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED
