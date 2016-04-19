//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_CONTAINERS_CONTAINERS_FWD_HPP
#define AUTOBOOST_INTERPROCESS_CONTAINERS_CONTAINERS_FWD_HPP

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

//////////////////////////////////////////////////////////////////////////////
//                        Standard predeclarations
//////////////////////////////////////////////////////////////////////////////

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/container/container_fwd.hpp>

namespace autoboost {
namespace interprocess {

using autoboost::container::ordered_range;
using autoboost::container::ordered_unique_range;

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

#endif //   #ifndef  AUTOBOOST_INTERPROCESS_CONTAINERS_CONTAINERS_FWD_HPP
