/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_STLPORT_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_STLPORT_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_STLPORT`]

[@http://sourceforge.net/projects/stlport/ STLport Standard C++] library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SGI_STL_PORT`] [__predef_detection__]]
    [[`_STLPORT_VERSION`] [__predef_detection__]]

    [[`_STLPORT_MAJOR`, `_STLPORT_MINOR`, `_STLPORT_PATCHLEVEL`] [V.R.P]]
    [[`_STLPORT_VERSION`] [V.R.P]]
    [[`__SGI_STL_PORT`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_STLPORT AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#   undef AUTOBOOST_LIB_STD_STLPORT
#   if !defined(AUTOBOOST_LIB_STD_STLPORT) && defined(_STLPORT_MAJOR)
#       define AUTOBOOST_LIB_STD_STLPORT \
            AUTOBOOST_VERSION_NUMBER(_STLPORT_MAJOR,_STLPORT_MINOR,_STLPORT_PATCHLEVEL)
#   endif
#   if !defined(AUTOBOOST_LIB_STD_STLPORT) && defined(_STLPORT_VERSION)
#       define AUTOBOOST_LIB_STD_STLPORT AUTOBOOST_PREDEF_MAKE_0X_VRP(_STLPORT_VERSION)
#   endif
#   if !defined(AUTOBOOST_LIB_STD_STLPORT)
#       define AUTOBOOST_LIB_STD_STLPORT AUTOBOOST_PREDEF_MAKE_0X_VRP(__SGI_STL_PORT)
#   endif
#endif

#if AUTOBOOST_LIB_STD_STLPORT
#   define AUTOBOOST_LIB_STD_STLPORT_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_STLPORT_NAME "STLport"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_STLPORT,AUTOBOOST_LIB_STD_STLPORT_NAME)
