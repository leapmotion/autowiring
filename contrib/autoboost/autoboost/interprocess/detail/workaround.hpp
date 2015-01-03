//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_WORKAROUND_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_WORKAROUND_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
   #define AUTOBOOST_INTERPROCESS_WINDOWS
   #define AUTOBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION
   #define AUTOBOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME
   //Define this to connect with shared memory created with versions < 1.54
   //#define AUTOBOOST_INTERPROCESS_BOOTSTAMP_IS_LASTBOOTUPTIME
#else
   #include <unistd.h>

   #if defined(_POSIX_THREAD_PROCESS_SHARED) && ((_POSIX_THREAD_PROCESS_SHARED - 0) > 0)
      //Cygwin defines _POSIX_THREAD_PROCESS_SHARED but does not implement it.
      #if defined(__CYGWIN__)
         #define AUTOBOOST_INTERPROCESS_BUGGY_POSIX_PROCESS_SHARED
      //Mac Os X < Lion (10.7) might define _POSIX_THREAD_PROCESS_SHARED but there is no real support.
      #elif defined(__APPLE__)
         #include "TargetConditionals.h"
         //Check we're on Mac OS target
         #if defined(TARGET_OS_MAC)
            #include "AvailabilityMacros.h"
            //If minimum target for this compilation is older than Mac Os Lion, then we are out of luck
            #if MAC_OS_X_VERSION_MIN_REQUIRED < 1070
               #define AUTOBOOST_INTERPROCESS_BUGGY_POSIX_PROCESS_SHARED
            #endif
         #endif
      #endif

      //If buggy _POSIX_THREAD_PROCESS_SHARED is detected avoid using it
      #if defined(AUTOBOOST_INTERPROCESS_BUGGY_POSIX_PROCESS_SHARED)
         #undef AUTOBOOST_INTERPROCESS_BUGGY_POSIX_PROCESS_SHARED
      #else
         #define AUTOBOOST_INTERPROCESS_POSIX_PROCESS_SHARED
      #endif
   #endif

   #if defined(_POSIX_BARRIERS) && ((_POSIX_BARRIERS - 0) > 0)
      #define AUTOBOOST_INTERPROCESS_POSIX_BARRIERS
   #endif

   #if defined(_POSIX_SEMAPHORES) && ((_POSIX_SEMAPHORES - 0) > 0)
      #define AUTOBOOST_INTERPROCESS_POSIX_NAMED_SEMAPHORES
      #if defined(__CYGWIN__)
         #define AUTOBOOST_INTERPROCESS_POSIX_SEMAPHORES_NO_UNLINK
      #endif
   //Some platforms have a limited (name length) named semaphore support
   #elif (defined(__FreeBSD__) && (__FreeBSD__ >= 4)) || defined(__APPLE__)
      #define AUTOBOOST_INTERPROCESS_POSIX_NAMED_SEMAPHORES
   #endif

   #if (defined (_V6_ILP32_OFFBIG)  &&(_V6_ILP32_OFFBIG   - 0 > 0)) ||\
       (defined (_V6_LP64_OFF64)    &&(_V6_LP64_OFF64     - 0 > 0)) ||\
       (defined (_V6_LPBIG_OFFBIG)  &&(_V6_LPBIG_OFFBIG   - 0 > 0)) ||\
       (defined (_XBS5_ILP32_OFFBIG)&&(_XBS5_ILP32_OFFBIG - 0 > 0)) ||\
       (defined (_XBS5_LP64_OFF64)  &&(_XBS5_LP64_OFF64   - 0 > 0)) ||\
       (defined (_XBS5_LPBIG_OFFBIG)&&(_XBS5_LPBIG_OFFBIG - 0 > 0)) ||\
       (defined (_FILE_OFFSET_BITS) &&(_FILE_OFFSET_BITS  - 0 >= 64))||\
       (defined (_FILE_OFFSET_BITS) &&(_FILE_OFFSET_BITS  - 0 >= 64))
      #define AUTOBOOST_INTERPROCESS_UNIX_64_BIT_OR_BIGGER_OFF_T
   #endif

   //Check for XSI shared memory objects. They are available in nearly all UNIX platforms
   #if !defined(__QNXNTO__) && !defined(__ANDROID__)
      #define AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS
   #endif

   #if defined(_POSIX_SHARED_MEMORY_OBJECTS) && ((_POSIX_SHARED_MEMORY_OBJECTS - 0) > 0)
      #define AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS
   #else
      //VMS and MACOS don't define it but they have shm_open/close interface
      #if defined(__vms)
         #if __CRTL_VER >= 70200000
            #define AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS
         #endif
         //Mac OS has some non-conformant features like names limited to SHM_NAME_MAX
      #elif defined (__APPLE__)
         //#define AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS
         //#define AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS_NO_GROW
      #endif
   #endif

   //Now check if we have only XSI shared memory
   #if defined(AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS) &&\
      !defined(AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS)
      //#define AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS_ONLY
   #endif

   #if defined(_POSIX_TIMEOUTS) && ((_POSIX_TIMEOUTS - 0) > 0)
      #define AUTOBOOST_INTERPROCESS_POSIX_TIMEOUTS
   #endif

   #ifdef AUTOBOOST_INTERPROCESS_POSIX_SHARED_MEMORY_OBJECTS
      //Some systems have filesystem-based resources, so the
      //portable "/shmname" format does not work due to permission issues
      //For those systems we need to form a path to a temporary directory:
      //          hp-ux               tru64               vms               freebsd
      #if defined(__hpux) || defined(__osf__) || defined(__vms) || (defined(__FreeBSD__) && (__FreeBSD__ < 7))
         #define AUTOBOOST_INTERPROCESS_FILESYSTEM_BASED_POSIX_SHARED_MEMORY
      //Some systems have "jailed" environments where shm usage is restricted at runtime
      //and temporary file file based shm is possible in those executions.
      #elif defined(__FreeBSD__)
         #define AUTOBOOST_INTERPROCESS_RUNTIME_FILESYSTEM_BASED_POSIX_SHARED_MEMORY
      #endif
   #endif

   #ifdef AUTOBOOST_INTERPROCESS_POSIX_NAMED_SEMAPHORES
      #if defined(__osf__) || defined(__vms)
         #define AUTOBOOST_INTERPROCESS_FILESYSTEM_BASED_POSIX_SEMAPHORES
      #endif
   #endif

   #if defined(_POSIX_VERSION) && defined(_XOPEN_VERSION) && \
       (((_POSIX_VERSION + 0)>= 200112L || (_XOPEN_VERSION + 0)>= 500))
      #define AUTOBOOST_INTERPROCESS_POSIX_RECURSIVE_MUTEXES
   #endif

   #if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
      #define AUTOBOOST_INTERPROCESS_BSD_DERIVATIVE
      //Some *BSD systems (OpenBSD & NetBSD) need sys/param.h before sys/sysctl.h, whereas
      //others (FreeBSD & Darwin) need sys/types.h
      #include <sys/types.h>
      #include <sys/param.h>
      #include <sys/sysctl.h>
      #if defined(CTL_KERN) && defined (KERN_BOOTTIME)
         //#define AUTOBOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME
      #endif
   #endif
#endif   //!defined(AUTOBOOST_INTERPROCESS_WINDOWS)

#if    !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   #define AUTOBOOST_INTERPROCESS_PERFECT_FORWARDING
#endif

//Now declare some Boost.Interprocess features depending on the implementation
#if defined(AUTOBOOST_INTERPROCESS_POSIX_NAMED_SEMAPHORES) && !defined(AUTOBOOST_INTERPROCESS_POSIX_SEMAPHORES_NO_UNLINK)
   #define AUTOBOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES
   #define AUTOBOOST_INTERPROCESS_NAMED_SEMAPHORE_USES_POSIX_SEMAPHORES
#endif

// Timeout duration use if AUTOBOOST_INTERPROCESS_ENABLE_TIMEOUT_WHEN_LOCKING is set
#ifndef AUTOBOOST_INTERPROCESS_TIMEOUT_WHEN_LOCKING_DURATION_MS
   #define AUTOBOOST_INTERPROCESS_TIMEOUT_WHEN_LOCKING_DURATION_MS 10000
#endif

//Other switches
//AUTOBOOST_INTERPROCESS_MSG_QUEUE_USES_CIRC_INDEX
//message queue uses a circular queue as index instead of an array (better performance)
//Boost version < 1.52 uses an array, so undef this if you want to communicate
//with processes compiled with those versions.
#define AUTOBOOST_INTERPROCESS_MSG_QUEUE_CIRCULAR_INDEX

//Inline attributes
#if defined(_MSC_VER)
   #define AUTOBOOST_INTERPROCESS_ALWAYS_INLINE __forceinline
#elif defined (__GNUC__)
   #define AUTOBOOST_INTERPROCESS_ALWAYS_INLINE __attribute__((__always_inline__))
#else
   #define AUTOBOOST_INTERPROCESS_ALWAYS_INLINE inline
#endif

#if defined(_MSC_VER)
   #define AUTOBOOST_INTERPROCESS_NEVER_INLINE __declspec(noinline)
#elif defined (__GNUC__)
   #define AUTOBOOST_INTERPROCESS_NEVER_INLINE __attribute__((__noinline__))
#endif

//Macros for documentation purposes. For code, expands to the argument
#define AUTOBOOST_INTERPROCESS_IMPDEF(TYPE) TYPE
#define AUTOBOOST_INTERPROCESS_SEEDOC(TYPE) TYPE

#if defined(AUTOBOOST_NO_CXX11_NOEXCEPT)
   #if defined(AUTOBOOST_MSVC)
      #define AUTOBOOST_INTERPROCESS_NOEXCEPT throw()
   #else
      #define AUTOBOOST_INTERPROCESS_NOEXCEPT
   #endif
   #define AUTOBOOST_INTERPROCESS_NOEXCEPT_IF(x)
#else
   #define AUTOBOOST_INTERPROCESS_NOEXCEPT    noexcept
   #define AUTOBOOST_INTERPROCESS_NOEXCEPT_IF(x) noexcept(x)
#endif

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DETAIL_WORKAROUND_HPP
