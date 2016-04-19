/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_ALIGNED_ALLOC_HPP
#define AUTOBOOST_ALIGN_ALIGNED_ALLOC_HPP

#include <autoboost/config.hpp>

#if defined(AUTOBOOST_HAS_UNISTD_H)
#include <unistd.h>
#endif

#if defined(__APPLE__) || defined(__APPLE_CC__) || defined(macintosh)
#include <AvailabilityMacros.h>
#endif

#if defined(_MSC_VER)
#include <autoboost/align/detail/aligned_alloc_msvc.hpp>
#elif defined(__MINGW32__) && (__MSVCRT_VERSION__ >= 0x0700)
#include <autoboost/align/detail/aligned_alloc_msvc.hpp>
#elif MAC_OS_X_VERSION_MIN_REQUIRED >= 1090
#include <autoboost/align/detail/aligned_alloc_posix.hpp>
#elif MAC_OS_X_VERSION_MIN_REQUIRED >= 1060
#include <autoboost/align/detail/aligned_alloc_macos.hpp>
#elif defined(__ANDROID__)
#include <autoboost/align/detail/aligned_alloc_android.hpp>
#elif defined(__SunOS_5_11) || defined(__SunOS_5_12)
#include <autoboost/align/detail/aligned_alloc_posix.hpp>
#elif defined(sun) || defined(__sun)
#include <autoboost/align/detail/aligned_alloc_sunos.hpp>
#elif (_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)
#include <autoboost/align/detail/aligned_alloc_posix.hpp>
#else
#include <autoboost/align/detail/aligned_alloc.hpp>
#endif

#endif
