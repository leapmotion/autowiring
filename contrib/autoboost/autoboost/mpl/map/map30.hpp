
#ifndef AUTOBOOST_MPL_MAP_MAP30_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_MAP30_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright David Abrahams 2003-2004
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
#   include <autoboost/mpl/map/map20.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER map30.hpp
#   include <autoboost/mpl/map/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/preprocessor/iterate.hpp>

namespace autoboost { namespace mpl {

#   define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(21, 30, <autoboost/mpl/map/aux_/numbered.hpp>))
#   include AUTOBOOST_PP_ITERATE()

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#endif // AUTOBOOST_MPL_MAP_MAP30_HPP_INCLUDED
