//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_CONTAINERS_ALLOCATION_TYPE_HPP
#define AUTOBOOST_INTERPROCESS_CONTAINERS_ALLOCATION_TYPE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/container/detail/allocation_type.hpp>

namespace autoboost {
namespace interprocess {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
typedef int allocation_type;
#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
static const allocation_type allocate_new       = autoboost::container::allocate_new;
static const allocation_type expand_fwd         = autoboost::container::expand_fwd;
static const allocation_type expand_bwd         = autoboost::container::expand_bwd;
static const allocation_type shrink_in_place    = autoboost::container::shrink_in_place;
static const allocation_type try_shrink_in_place= autoboost::container::try_shrink_in_place;
static const allocation_type nothrow_allocation = autoboost::container::nothrow_allocation;
static const allocation_type zero_memory        = autoboost::container::zero_memory;

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif //   #ifndef  AUTOBOOST_INTERPROCESS_CONTAINERS_ALLOCATION_TYPE_HPP
