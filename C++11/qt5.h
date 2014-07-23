// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef __QT5_H__
#define __QT5_H__

#include <QtCore/qcompilerdetection.h>

#ifdef __APPLE__
#include <ciso646>  // detect std::lib
#ifndef _LIBCPP_VERSION // clang + libstdc++
// Qt's compiler detection assumes that if we are using C++11, that we are also
// using libc++. When building with libstdc++, undefine the following macros
// which use dependent libraries:
#undef Q_COMPILER_INITIALIZER_LISTS
#undef Q_COMPILER_RVALUE_REFS
#endif
#endif

#endif // __QT5_H__
