
#ifndef AUTOBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)
#   define AUTOBOOST_MSVC_TYPENAME
#else
#   define AUTOBOOST_MSVC_TYPENAME typename
#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED
