//  Boost compiler configuration selection header file

//  (C) Copyright John Maddock 2001 - 2002.
//  (C) Copyright Jens Maurer 2001.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

// locate which platform we are on and define AUTOBOOST_PLATFORM_CONFIG as needed.
// Note that we define the headers to include using "header_name" not
// <header_name> in order to prevent macro expansion within the header
// name (for example "linux" is a macro on linux systems).

#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
// linux, also other platforms (Hurd etc) that use GLIBC, should these really have their own config headers though?
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/linux.hpp"

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
// BSD:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/bsd.hpp"

#elif defined(sun) || defined(__sun)
// solaris:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/solaris.hpp"

#elif defined(__sgi)
// SGI Irix:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/irix.hpp"

#elif defined(__hpux)
// hp unix:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/hpux.hpp"

#elif defined(__CYGWIN__)
// cygwin is not win32:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/cygwin.hpp"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// win32:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/win32.hpp"

#elif defined(__HAIKU__)
// Haiku
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/haiku.hpp"

#elif defined(__BEOS__)
// BeOS
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/beos.hpp"

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
// MacOS
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/macos.hpp"

#elif defined(__IBMCPP__) || defined(_AIX)
// IBM
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/aix.hpp"

#elif defined(__amigaos__)
// AmigaOS
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/amigaos.hpp"

#elif defined(__QNXNTO__)
// QNX:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/qnxnto.hpp"

#elif defined(__VXWORKS__)
// vxWorks:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/vxworks.hpp"

#elif defined(__SYMBIAN32__)
// Symbian:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/symbian.hpp"

#elif defined(_CRAYC)
// Cray:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/cray.hpp"

#elif defined(__VMS)
// VMS:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/vms.hpp"

#elif defined(__CloudABI__)
// Nuxi CloudABI:
#  define AUTOBOOST_PLATFORM_CONFIG "autoboost/config/platform/cloudabi.hpp"
#else

#  if defined(unix) \
      || defined(__unix) \
      || defined(_XOPEN_SOURCE) \
      || defined(_POSIX_SOURCE)

   // generic unix platform:

#  ifndef AUTOBOOST_HAS_UNISTD_H
#     define AUTOBOOST_HAS_UNISTD_H
#  endif

#  include <autoboost/config/posix_features.hpp>

#  endif

#  if defined (AUTOBOOST_ASSERT_CONFIG)
      // this must come last - generate an error if we don't
      // recognise the platform:
#     error "Unknown platform - please configure and report the results to boost.org"
#  endif

#endif

#if 0
//
// This section allows dependency scanners to find all the files we *might* include:
//
#  include "autoboost/config/platform/linux.hpp"
#  include "autoboost/config/platform/bsd.hpp"
#  include "autoboost/config/platform/solaris.hpp"
#  include "autoboost/config/platform/irix.hpp"
#  include "autoboost/config/platform/hpux.hpp"
#  include "autoboost/config/platform/cygwin.hpp"
#  include "autoboost/config/platform/win32.hpp"
#  include "autoboost/config/platform/beos.hpp"
#  include "autoboost/config/platform/macos.hpp"
#  include "autoboost/config/platform/aix.hpp"
#  include "autoboost/config/platform/amigaos.hpp"
#  include "autoboost/config/platform/qnxnto.hpp"
#  include "autoboost/config/platform/vxworks.hpp"
#  include "autoboost/config/platform/symbian.hpp"
#  include "autoboost/config/platform/cray.hpp"
#  include "autoboost/config/platform/vms.hpp"
#  include <autoboost/config/posix_features.hpp>



#endif

