
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED
#define AUTOBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
// Copyright Peter Dimov 2001-2003
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
#   include <autoboost/mpl/arg.hpp>
#   include <autoboost/mpl/aux_/adl_barrier.hpp>

#   if !defined(AUTOBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE)
#       define AUTOBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(type) \
        using ::AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
        /**/
#   else
#       define AUTOBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(type) /**/
#   endif

#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
 && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER placeholders.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/aux_/nttp_decl.hpp>
#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/cat.hpp>

// watch out for GNU gettext users, who #define _(x)
#if !defined(_) || defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)
AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN
typedef arg<-1> _;
AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

namespace autoboost { namespace mpl {

AUTOBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(_)

namespace placeholders {
using AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::_;
}

}}
#endif

/// agurt, 17/mar/02: one more placeholder for the last 'apply#'
/// specialization
#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(1, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY + 1, <autoboost/mpl/placeholders.hpp>))
#include AUTOBOOST_PP_ITERATE()

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED

///// iteration

#else
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

typedef arg<i_> AUTOBOOST_PP_CAT(_,i_);

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

namespace autoboost { namespace mpl {

AUTOBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(AUTOBOOST_PP_CAT(_,i_))

namespace placeholders {
using AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::AUTOBOOST_PP_CAT(_,i_);
}

}}

#undef i_
#endif // AUTOBOOST_PP_IS_ITERATING
