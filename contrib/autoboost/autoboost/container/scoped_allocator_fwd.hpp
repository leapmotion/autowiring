//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP
#define AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP

//! \file
//! This header file forward declares autoboost::container::scoped_allocator_adaptor

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/container/detail/std_fwd.hpp>
#include <autoboost/container/uses_allocator_fwd.hpp>

#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif

namespace autoboost { namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)

      template <typename OuterAlloc, typename ...InnerAllocs>
      class scoped_allocator_adaptor;

   #else // #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)

      template <typename ...InnerAllocs>
      class scoped_allocator_adaptor;

      template <typename OuterAlloc, typename ...InnerAllocs>
      class scoped_allocator_adaptor<OuterAlloc, InnerAllocs...>;

   #endif   // #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)

#else    // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template <typename OuterAlloc, AUTOBOOST_MOVE_CLASSDFLT9>
   class scoped_allocator_adaptor;

#endif


#else    //AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

}} // namespace autoboost { namespace container {

#include <autoboost/container/detail/config_end.hpp>

#endif //  AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP
