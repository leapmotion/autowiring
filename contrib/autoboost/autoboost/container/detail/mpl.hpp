//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP
#define AUTOBOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/move/detail/type_traits.hpp>
#include <autoboost/intrusive/detail/mpl.hpp>

#include <cstddef>

namespace autoboost {
namespace container {
namespace container_detail {

using autoboost::move_detail::integral_constant;
using autoboost::move_detail::true_type;
using autoboost::move_detail::false_type;
using autoboost::move_detail::enable_if_c;
using autoboost::move_detail::enable_if;
using autoboost::move_detail::enable_if_convertible;
using autoboost::move_detail::disable_if_c;
using autoboost::move_detail::disable_if;
using autoboost::move_detail::disable_if_convertible;
using autoboost::move_detail::is_convertible;
using autoboost::move_detail::if_c;
using autoboost::move_detail::if_;
using autoboost::move_detail::identity;
using autoboost::move_detail::bool_;
using autoboost::move_detail::true_;
using autoboost::move_detail::false_;
using autoboost::move_detail::yes_type;
using autoboost::move_detail::no_type;
using autoboost::move_detail::bool_;
using autoboost::move_detail::true_;
using autoboost::move_detail::false_;
using autoboost::move_detail::unvoid_ref;
using autoboost::move_detail::and_;
using autoboost::move_detail::or_;
using autoboost::move_detail::not_;
using autoboost::move_detail::enable_if_and;
using autoboost::move_detail::disable_if_and;
using autoboost::move_detail::enable_if_or;
using autoboost::move_detail::disable_if_or;

template <class FirstType>
struct select1st
{
   typedef FirstType type;

   template<class T>
   const type& operator()(const T& x) const
   {  return x.first;   }

   template<class T>
   type& operator()(T& x)
   {  return const_cast<type&>(x.first);   }
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_CONTAINER_CONTAINER_DETAIL_MPL_HPP

