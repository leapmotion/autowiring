//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_NULL_MUTEX_HPP
#define AUTOBOOST_INTERPROCESS_NULL_MUTEX_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>


//!\file
//!Describes null_mutex classes

namespace autoboost {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

namespace posix_time
{  class ptime;   }

#endif   //#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

namespace interprocess {

//!Implements a mutex that simulates a mutex without doing any operation and
//!simulates a successful operation.
class null_mutex
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   null_mutex(const null_mutex&);
   null_mutex &operator= (const null_mutex&);
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
   public:

   //!Constructor.
   //!Empty.
   null_mutex(){}

   //!Destructor.
   //!Empty.
   ~null_mutex(){}

   //!Simulates a mutex lock() operation. Empty function.
   void lock(){}

   //!Simulates a mutex try_lock() operation.
   //!Equivalent to "return true;"
   bool try_lock()
   {  return true;   }

   //!Simulates a mutex timed_lock() operation.
   //!Equivalent to "return true;"
   bool timed_lock(const autoboost::posix_time::ptime &)
   {  return true;   }

   //!Simulates a mutex unlock() operation.
   //!Empty function.
   void unlock(){}

   //!Simulates a mutex lock_sharable() operation.
   //!Empty function.
   void lock_sharable(){}

   //!Simulates a mutex try_lock_sharable() operation.
   //!Equivalent to "return true;"
   bool try_lock_sharable()
   {  return true;   }

   //!Simulates a mutex timed_lock_sharable() operation.
   //!Equivalent to "return true;"
   bool timed_lock_sharable(const autoboost::posix_time::ptime &)
   {  return true;   }

   //!Simulates a mutex unlock_sharable() operation.
   //!Empty function.
   void unlock_sharable(){}

   //!Simulates a mutex lock_upgradable() operation.
   //!Empty function.
   void lock_upgradable(){}

   //!Simulates a mutex try_lock_upgradable() operation.
   //!Equivalent to "return true;"
   bool try_lock_upgradable()
   {  return true;   }

   //!Simulates a mutex timed_lock_upgradable() operation.
   //!Equivalent to "return true;"
   bool timed_lock_upgradable(const autoboost::posix_time::ptime &)
   {  return true;   }

   //!Simulates a mutex unlock_upgradable() operation.
   //!Empty function.
   void unlock_upgradable(){}

   //!Simulates unlock_and_lock_upgradable().
   //!Empty function.
   void unlock_and_lock_upgradable(){}

   //!Simulates unlock_and_lock_sharable().
   //!Empty function.
   void unlock_and_lock_sharable(){}

   //!Simulates unlock_upgradable_and_lock_sharable().
   //!Empty function.
   void unlock_upgradable_and_lock_sharable(){}

   //Promotions

   //!Simulates unlock_upgradable_and_lock().
   //!Empty function.
   void unlock_upgradable_and_lock(){}

   //!Simulates try_unlock_upgradable_and_lock().
   //!Equivalent to "return true;"
   bool try_unlock_upgradable_and_lock()
   {  return true;   }

   //!Simulates timed_unlock_upgradable_and_lock().
   //!Equivalent to "return true;"
   bool timed_unlock_upgradable_and_lock(const autoboost::posix_time::ptime &)
   {  return true;   }

   //!Simulates try_unlock_sharable_and_lock().
   //!Equivalent to "return true;"
   bool try_unlock_sharable_and_lock()
   {  return true;   }

   //!Simulates try_unlock_sharable_and_lock_upgradable().
   //!Equivalent to "return true;"
   bool try_unlock_sharable_and_lock_upgradable()
   {  return true;   }
};

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_NULL_MUTEX_HPP
