//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_NAMED_MUTEX_HPP
#define AUTOBOOST_INTERPROCESS_NAMED_MUTEX_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>
#include <autoboost/interprocess/creation_tags.hpp>
#include <autoboost/interprocess/exceptions.hpp>
#include <autoboost/interprocess/detail/interprocess_tester.hpp>
#include <autoboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <autoboost/interprocess/permissions.hpp>

#if defined(AUTOBOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)
   #include <autoboost/interprocess/sync/posix/named_mutex.hpp>
   #define AUTOBOOST_INTERPROCESS_USE_POSIX_SEMAPHORES
#elif !defined(AUTOBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION) && defined (AUTOBOOST_INTERPROCESS_WINDOWS)
   #include <autoboost/interprocess/sync/windows/named_mutex.hpp>
   #define AUTOBOOST_INTERPROCESS_USE_WINDOWS
#else
#include <autoboost/interprocess/sync/shm/named_mutex.hpp>
#endif

//!\file
//!Describes a named mutex class for inter-process synchronization

namespace autoboost {
namespace interprocess {

class named_condition;

//!A mutex with a global name, so it can be found from different
//!processes. This mutex can't be placed in shared memory, and
//!each process should have it's own named_mutex.
class named_mutex
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

   //Non-copyable
   named_mutex();
   named_mutex(const named_mutex &);
   named_mutex &operator=(const named_mutex &);
   friend class named_condition;
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

   public:
   //!Creates a global mutex with a name.
   //!Throws interprocess_exception on error.
   named_mutex(create_only_t create_only, const char *name, const permissions &perm = permissions());

   //!Opens or creates a global mutex with a name.
   //!If the mutex is created, this call is equivalent to
   //!named_mutex(create_only_t, ... )
   //!If the mutex is already created, this call is equivalent
   //!named_mutex(open_only_t, ... )
   //!Does not throw
   named_mutex(open_or_create_t open_or_create, const char *name, const permissions &perm = permissions());

   //!Opens a global mutex with a name if that mutex is previously
   //!created. If it is not previously created this function throws
   //!interprocess_exception.
   named_mutex(open_only_t open_only, const char *name);

   //!Destroys *this and indicates that the calling process is finished using
   //!the resource. The destructor function will deallocate
   //!any system resources allocated by the system for use by this process for
   //!this resource. The resource can still be opened again calling
   //!the open constructor overload. To erase the resource from the system
   //!use remove().
   ~named_mutex();

   //!Unlocks a previously locked
   //!mutex.
   void unlock();

   //!Locks the mutex, sleeps when the mutex is already locked.
   //!Throws interprocess_exception if a severe error is found
   void lock();

   //!Tries to lock the mutex, returns false when the mutex
   //!is already locked, returns true when success.
   //!Throws interprocess_exception if a severe error is found
   bool try_lock();

   //!Tries to lock the the mutex until time abs_time,
   //!Returns false when timeout expires, returns true when locks.
   //!Throws interprocess_exception if a severe error is found
   bool timed_lock(const autoboost::posix_time::ptime &abs_time);

   //!Erases a named mutex from the system.
   //!Returns false on error. Never throws.
   static bool remove(const char *name);

   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   private:
   friend class ipcdetail::interprocess_tester;
   void dont_close_on_destruction();

   public:
   #if defined(AUTOBOOST_INTERPROCESS_USE_POSIX_SEMAPHORES)
      typedef ipcdetail::posix_named_mutex      internal_mutex_type;
      #undef AUTOBOOST_INTERPROCESS_USE_POSIX_SEMAPHORES
   #elif defined(AUTOBOOST_INTERPROCESS_USE_WINDOWS)
      typedef ipcdetail::windows_named_mutex    internal_mutex_type;
      #undef AUTOBOOST_INTERPROCESS_USE_WINDOWS
   #else
      typedef ipcdetail::shm_named_mutex        internal_mutex_type;
   #endif
   internal_mutex_type &internal_mutex()
   {  return m_mut; }

   internal_mutex_type m_mut;

   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
};

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

inline named_mutex::named_mutex(create_only_t, const char *name, const permissions &perm)
   :  m_mut(create_only_t(), name, perm)
{}

inline named_mutex::named_mutex(open_or_create_t, const char *name, const permissions &perm)
   :  m_mut(open_or_create_t(), name, perm)
{}

inline named_mutex::named_mutex(open_only_t, const char *name)
   :  m_mut(open_only_t(), name)
{}

inline void named_mutex::dont_close_on_destruction()
{  ipcdetail::interprocess_tester::dont_close_on_destruction(m_mut); }

inline named_mutex::~named_mutex()
{}

inline void named_mutex::lock()
{  m_mut.lock();  }

inline void named_mutex::unlock()
{  m_mut.unlock();  }

inline bool named_mutex::try_lock()
{  return m_mut.try_lock();  }

inline bool named_mutex::timed_lock(const autoboost::posix_time::ptime &abs_time)
{  return m_mut.timed_lock(abs_time);  }

inline bool named_mutex::remove(const char *name)
{  return internal_mutex_type::remove(name);   }

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_NAMED_MUTEX_HPP
