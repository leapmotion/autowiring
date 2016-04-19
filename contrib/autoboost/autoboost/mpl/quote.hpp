
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_QUOTE_HPP_INCLUDED
#define AUTOBOOST_MPL_QUOTE_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/void.hpp>
#   include <autoboost/mpl/aux_/has_type.hpp>
#endif

#include <autoboost/mpl/aux_/config/bcc.hpp>
#include <autoboost/mpl/aux_/config/ttp.hpp>

#if defined(AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
    && !defined(AUTOBOOST_MPL_CFG_BCC590_WORKAROUNDS)
#   define AUTOBOOST_MPL_CFG_NO_QUOTE_TEMPLATE
#endif

#if !defined(AUTOBOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS) \
    && defined(AUTOBOOST_MPL_CFG_NO_HAS_XXX)
#   define AUTOBOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
 && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER quote.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/config/workaround.hpp>

#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/cat.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_QUOTE_TEMPLATE)

namespace autoboost { namespace mpl {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< typename T, bool has_type_ >
struct quote_impl
// GCC has a problem with metafunction forwarding when T is a
// specialization of a template called 'type'.
# if AUTOBOOST_WORKAROUND(__GNUC__, AUTOBOOST_TESTED_AT(4)) \
    && AUTOBOOST_WORKAROUND(__GNUC_MINOR__, AUTOBOOST_TESTED_AT(0)) \
    && AUTOBOOST_WORKAROUND(__GNUC_PATCHLEVEL__, AUTOBOOST_TESTED_AT(2))
{
    typedef typename T::type type;
};
# else
    : T
{
};
# endif

template< typename T >
struct quote_impl<T,false>
{
    typedef T type;
};

#else // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

template< bool > struct quote_impl
{
    template< typename T > struct result_
        : T
    {
    };
};

template<> struct quote_impl<false>
{
    template< typename T > struct result_
    {
        typedef T type;
    };
};

#endif

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(1, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/quote.hpp>))
#include AUTOBOOST_PP_ITERATE()

}}

#endif // AUTOBOOST_MPL_CFG_NO_QUOTE_TEMPLATE

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_QUOTE_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<
      template< AUTOBOOST_MPL_PP_PARAMS(i_, typename P) > class F
    , typename Tag = void_
    >
struct AUTOBOOST_PP_CAT(quote,i_)
{
    template< AUTOBOOST_MPL_PP_PARAMS(i_, typename U) > struct apply
#if defined(AUTOBOOST_MPL_CFG_BCC590_WORKAROUNDS)
    {
        typedef typename quote_impl<
              F< AUTOBOOST_MPL_PP_PARAMS(i_, U) >
            , aux::has_type< F< AUTOBOOST_MPL_PP_PARAMS(i_, U) > >::value
            >::type type;
    };
#elif !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
        : quote_impl<
              F< AUTOBOOST_MPL_PP_PARAMS(i_, U) >
            , aux::has_type< F< AUTOBOOST_MPL_PP_PARAMS(i_, U) > >::value
            >
    {
    };
#else
        : quote_impl< aux::has_type< F< AUTOBOOST_MPL_PP_PARAMS(i_, U) > >::value >
            ::template result_< F< AUTOBOOST_MPL_PP_PARAMS(i_, U) > >
    {
    };
#endif
};

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
