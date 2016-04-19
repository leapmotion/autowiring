//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
// Parts of the pthread code come from Boost Threads code.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_MUTEX_HPP
#define AUTOBOOST_INTERPROCESS_MUTEX_HPP

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/exceptions.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>
#include <autoboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <autoboost/assert.hpp>

#if !defined(AUTOBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION) && defined (AUTOBOOST_INTERPROCESS_POSIX_PROCESS_SHARED)
   #include <autoboost/interprocess/sync/posix/mutex.hpp>
   #define AUTOBOOST_INTERPROCESS_USE_POSIX
//Experimental...
#elif !defined(AUTOBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION) && defined (AUTOBOOST_INTERPROCESS_WINDOWS)
   #include <autoboost/interprocess/sync/windows/mutex.hpp>
   #define AUTOBOOST_INTERPROCESS_USE_WINDOWS
#elif !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   #include <autoboost/interprocess/sync/spin/mutex.hpp>
   #define AUTOBOOST_INTERPROCESS_USE_GENERIC_EMULATION

namespace autoboost {
namespace interprocess {
namespace ipcdetail{
namespace robust_emulation_helpers {

template<class T>
class mutex_traits;

}}}}

#endif

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

//!\file
//!Describes a mutex class that can be placed in memory shared by
//!several processes.

namespace autoboost {
namespace interprocess {

class interprocess_condition;

//!Wraps a interprocess_mutex that can be placed in shared memory and can be
//!shared between processes. Allows timed lock tries
class interprocess_mutex
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   //Non-copyable
   interprocess_mutex(const interprocess_mutex &);
   interprocess_mutex &operator=(const interprocess_mutex &);
   friend class interprocess_condition;

   public:
   #if defined(AUTOBOOST_INTERPROCESS_USE_GENERIC_EMULATION)
      #undef AUTOBOOST_INTERPROCESS_USE_GENERIC_EMULATION
      typedef ipcdetail::spin_mutex internal_mutex_type;
      private:
      friend class ipcdetail::robust_emulation_helpers::mutex_traits<interprocess_mutex>;
      void take_ownership(){ m_mutex.take_ownership(); }
      public:
   #elif defined(AUTOBOOST_INTERPROCESS_USE_POSIX)
      #undef AUTOBOOST_INTERPROCESS_USE_POSIX
      typedef ipcdetail::posix_mutex internal_mutex_type;
   #elif defined(AUTOBOOST_INTERPROCESS_USE_WINDOWS)
      #undef AUTOBOOST_INTERPROCESS_USE_WINDOWS
      typedef ipcdetail::windows_mutex internal_mutex_type;
   #else
      #error "Unknown platform for interprocess_mutex"
   #endif

   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
   public:

   //!Constructor.
   //!Throws interprocess_exception on error.
   interprocess_mutex();

   //!Destructor. If any process uses the mutex after the destructor is called
   //!the result is undefined. Does not throw.
   ~interprocess_mutex();

   //!Effects: The calling thread tries to obtain ownership of the mutex, and
   //!   if another thread has ownership of the mutex, it waits until it can
   //!   obtain the ownership. If a thread takes ownership of the mutex the
   //!   mutex must be unlocked by the same mutex.
   //!Throws: interprocess_exception on error.
   void lock();

   //!Effects: The calling thread tries to obtain ownership of the mutex, and
   //!   if another thread has ownership of the mutex returns immediately.
   //!Returns: If the thread acquires ownership of the mutex, returns true, if
   //!   the another thread has ownership of the mutex, returns false.
   //!Throws: interprocess_exception on error.
   bool try_lock();

   //!Effects: The calling thread will try to obtain exclusive ownership of the
   //!   mutex if it can do so in until the specified time is reached. If the
   //!   mutex supports recursive locking, the mutex must be unlocked the same
   //!   number of times it is locked.
   //!Returns: If the thread acquires ownership of the mutex, returns true, if
   //!   the timeout expires returns false.
   //!Throws: interprocess_exception on error.
   bool timed_lock(const autoboost::posix_time::ptime &abs_time);

   //!Effects: The calling thread releases the exclusive ownership of the mutex.
   //!Throws: interprocess_exception on error.
   void unlock();

   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   internal_mutex_type &internal_mutex()
   {  return m_mutex;   }

   const internal_mutex_type &internal_mutex() const
   {  return m_mutex;   }

   private:
   internal_mutex_type m_mutex;
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
};

}  //namespace interprocess {
}  //namespace autoboost {


namespace autoboost {
namespace interprocess {

inline interprocess_mutex::interprocess_mutex(){}

inline interprocess_mutex::~interprocess_mutex(){}

inline void interprocess_mutex::lock()
{
   #ifdef AUTOBOOST_INTERPROCESS_ENABLE_TIMEOUT_WHEN_LOCKING
      autoboost::posix_time::ptime wait_time
         = microsec_clock::universal_time()
         + autoboost::posix_time::milliseconds(AUTOBOOST_INTERPROCESS_TIMEOUT_WHEN_LOCKING_DURATION_MS);
      if (!m_mutex.timed_lock(wait_time))
      {
         throw interprocess_exception(timeout_when_locking_error
                                     , "Interprocess mutex timeout when locking. Possible deadlock: "
                                       "owner died without unlocking?");
      }
   #else
      m_mutex.lock();
   #endif
}

inline bool interprocess_mutex::try_lock()
{ return m_mutex.try_lock(); }

inline bool interprocess_mutex::timed_lock(const autoboost::posix_time::ptime &abs_time)
{ return m_mutex.timed_lock(abs_time); }

inline void interprocess_mutex::unlock()
{ m_mutex.unlock(); }

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_MUTEX_HPP
