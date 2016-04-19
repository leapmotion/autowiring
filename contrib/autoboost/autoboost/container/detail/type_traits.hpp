//////////////////////////////////////////////////////////////////////////////
// (C) Copyright John Maddock 2000.
// (C) Copyright Ion Gaztanaga 2005-2015.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
// The alignment and Type traits implementation comes from
// John Maddock's TypeTraits library.
//
// Some other tricks come from Howard Hinnant's papers and StackOverflow replies
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP
#define AUTOBOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/move/detail/type_traits.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

using ::autoboost::move_detail::enable_if;
using ::autoboost::move_detail::enable_if_and;
using ::autoboost::move_detail::is_same;
using ::autoboost::move_detail::is_different;
using ::autoboost::move_detail::is_pointer;
using ::autoboost::move_detail::add_reference;
using ::autoboost::move_detail::add_const;
using ::autoboost::move_detail::add_const_reference;
using ::autoboost::move_detail::remove_const;
using ::autoboost::move_detail::remove_reference;
using ::autoboost::move_detail::make_unsigned;
using ::autoboost::move_detail::is_floating_point;
using ::autoboost::move_detail::is_integral;
using ::autoboost::move_detail::is_enum;
using ::autoboost::move_detail::is_pod;
using ::autoboost::move_detail::is_empty;
using ::autoboost::move_detail::is_trivially_destructible;
using ::autoboost::move_detail::is_trivially_default_constructible;
using ::autoboost::move_detail::is_trivially_copy_constructible;
using ::autoboost::move_detail::is_trivially_move_constructible;
using ::autoboost::move_detail::is_trivially_copy_assignable;
using ::autoboost::move_detail::is_trivially_move_assignable;
using ::autoboost::move_detail::is_nothrow_default_constructible;
using ::autoboost::move_detail::is_nothrow_copy_constructible;
using ::autoboost::move_detail::is_nothrow_move_constructible;
using ::autoboost::move_detail::is_nothrow_copy_assignable;
using ::autoboost::move_detail::is_nothrow_move_assignable;
using ::autoboost::move_detail::is_nothrow_swappable;
using ::autoboost::move_detail::alignment_of;
using ::autoboost::move_detail::aligned_storage;
using ::autoboost::move_detail::nat;
using ::autoboost::move_detail::max_align_t;

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_CONTAINER_DETAIL_TYPE_TRAITS_HPP
