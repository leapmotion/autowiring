/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_MAP_FORWARD_MAIN_07212005_1105)
#define FUSION_MAP_FORWARD_MAIN_07212005_1105

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/config.hpp>

#if (defined(AUTOBOOST_NO_CXX11_DECLTYPE)             \
  || defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)   \
  || defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES))   \
  || (defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES))
# if defined(AUTOBOOST_FUSION_HAS_VARIADIC_MAP)
#   undef AUTOBOOST_FUSION_HAS_VARIADIC_MAP
# endif
#else
# if !defined(AUTOBOOST_FUSION_HAS_VARIADIC_MAP)
#   define AUTOBOOST_FUSION_HAS_VARIADIC_MAP
# endif
#endif

///////////////////////////////////////////////////////////////////////////////
// With no decltype and variadics, we will use the C++03 version
///////////////////////////////////////////////////////////////////////////////
#if !defined(AUTOBOOST_FUSION_HAS_VARIADIC_MAP)
# include <autoboost/fusion/container/map/detail/cpp03/map_fwd.hpp>
#else

#include <autoboost/fusion/container/map/detail/map_impl.hpp>

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
namespace autoboost { namespace fusion
{
    template <typename ...T>
    struct map;
}}

#endif
#endif
