//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_LOCKS_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_LOCKS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>

namespace autoboost {
namespace interprocess {
namespace ipcdetail {

template<class Lock>
class internal_mutex_lock
{
   typedef void (internal_mutex_lock::*unspecified_bool_type)();
   public:

   typedef typename Lock::mutex_type::internal_mutex_type  mutex_type;


   internal_mutex_lock(Lock &l)
      : l_(l)
   {}

   mutex_type* mutex() const
   {  return l_ ? &l_.mutex()->internal_mutex() : 0;  }

   void lock()    { l_.lock(); }

   void unlock()  { l_.unlock(); }

   operator unspecified_bool_type() const
   {  return l_ ? &internal_mutex_lock::lock : 0;  }

   private:
   Lock &l_;
};

template <class Lock>
class lock_inverter
{
   Lock &l_;
   public:
   lock_inverter(Lock &l)
      :  l_(l)
   {}
   void lock()    {   l_.unlock();   }
   void unlock()  {   l_.lock();     }
};

template <class Lock>
class lock_to_sharable
{
   Lock &l_;

   public:
   explicit lock_to_sharable(Lock &l)
      :  l_(l)
   {}
   void lock()    {  l_.lock_sharable();     }
   bool try_lock(){  return l_.try_lock_sharable(); }
   void unlock()  {  l_.unlock_sharable();   }
};

template <class Lock>
class lock_to_wait
{
   Lock &l_;

   public:
   explicit lock_to_wait(Lock &l)
      :  l_(l)
   {}
   void lock()    {  l_.wait();     }
   bool try_lock(){  return l_.try_wait(); }
};

}  //namespace ipcdetail
}  //namespace interprocess
}  //namespace autoboost

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_DETAIL_LOCKS_HPP
