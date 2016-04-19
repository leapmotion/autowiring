//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_DETAIL_ALLOC_TRAITS_HPP
#define AUTOBOOST_CONTAINER_DETAIL_ALLOC_TRAITS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

// move
#include <autoboost/move/adl_move_swap.hpp>
#include <autoboost/move/utility_core.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

template<class AllocatorType>
inline void swap_alloc(AllocatorType &, AllocatorType &, container_detail::false_type)
   AUTOBOOST_NOEXCEPT_OR_NOTHROW
{}

template<class AllocatorType>
inline void swap_alloc(AllocatorType &l, AllocatorType &r, container_detail::true_type)
{  autoboost::adl_move_swap(l, r);   }

template<class AllocatorType>
inline void assign_alloc(AllocatorType &, const AllocatorType &, container_detail::false_type)
   AUTOBOOST_NOEXCEPT_OR_NOTHROW
{}

template<class AllocatorType>
inline void assign_alloc(AllocatorType &l, const AllocatorType &r, container_detail::true_type)
{  l = r;   }

template<class AllocatorType>
inline void move_alloc(AllocatorType &, AllocatorType &, container_detail::false_type)
   AUTOBOOST_NOEXCEPT_OR_NOTHROW
{}

template<class AllocatorType>
inline void move_alloc(AllocatorType &l, AllocatorType &r, container_detail::true_type)
{  l = ::autoboost::move(r);   }

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_ALLOC_TRAITS_HPP
