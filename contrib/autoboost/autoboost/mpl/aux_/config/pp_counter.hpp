
#ifndef AUTOBOOST_MPL_AUX_CONFIG_PP_COUNTER_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_PP_COUNTER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2006
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#if !defined(AUTOBOOST_MPL_AUX_PP_COUNTER)
#   include <autoboost/mpl/aux_/config/msvc.hpp>
#   if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, >= 1300)
#       define AUTOBOOST_MPL_AUX_PP_COUNTER() __COUNTER__
#   else
#       define AUTOBOOST_MPL_AUX_PP_COUNTER() __LINE__
#   endif
#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_PP_COUNTER_HPP_INCLUDED
