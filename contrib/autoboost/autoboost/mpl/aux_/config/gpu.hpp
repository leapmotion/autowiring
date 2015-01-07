
#ifndef AUTOBOOST_MPL_AUX_CONFIG_GPU_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_GPU_HPP_INCLUDED

// Copyright Eric Niebler 2014
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/config.hpp>

#if !defined(AUTOBOOST_MPL_CFG_GPU_ENABLED) \

#   define AUTOBOOST_MPL_CFG_GPU_ENABLED AUTOBOOST_GPU_ENABLED

#endif

#if defined __CUDACC__

#    define AUTOBOOST_MPL_CFG_GPU 1

#else

#    define AUTOBOOST_MPL_CFG_GPU 0

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_GPU_HPP_INCLUDED
