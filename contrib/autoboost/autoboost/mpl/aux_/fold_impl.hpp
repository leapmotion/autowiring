
#ifndef AUTOBOOST_MPL_AUX_FOLD_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_FOLD_IMPL_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/next_prior.hpp>
#   include <autoboost/mpl/apply.hpp>
#   include <autoboost/mpl/deref.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#       include <autoboost/mpl/if.hpp>
#       include <autoboost/type_traits/is_same.hpp>
#   endif
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER fold_impl.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   define AUX778076_FOLD_IMPL_OP(iter) typename deref<iter>::type
#   define AUX778076_FOLD_IMPL_NAME_PREFIX fold
#   include <autoboost/mpl/aux_/fold_impl_body.hpp>

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_AUX_FOLD_IMPL_HPP_INCLUDED
