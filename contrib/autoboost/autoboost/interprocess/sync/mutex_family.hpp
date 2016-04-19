//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_MUTEX_FAMILY_HPP
#define AUTOBOOST_INTERPROCESS_MUTEX_FAMILY_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>

#include <autoboost/interprocess/sync/interprocess_mutex.hpp>
#include <autoboost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <autoboost/interprocess/sync/null_mutex.hpp>

//!\file
//!Describes a shared interprocess_mutex family fit algorithm used to allocate objects in shared memory.

namespace autoboost {

namespace interprocess {

//!Describes interprocess_mutex family to use with Interprocess framework
//!based on autoboost::interprocess synchronization objects.
struct mutex_family
{
   typedef autoboost::interprocess::interprocess_mutex                 mutex_type;
   typedef autoboost::interprocess::interprocess_recursive_mutex       recursive_mutex_type;
};

//!Describes interprocess_mutex family to use with Interprocess frameworks
//!based on null operation synchronization objects.
struct null_mutex_family
{
   typedef autoboost::interprocess::null_mutex                   mutex_type;
   typedef autoboost::interprocess::null_mutex                   recursive_mutex_type;
};

}  //namespace interprocess {

}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTERPROCESS_MUTEX_FAMILY_HPP


