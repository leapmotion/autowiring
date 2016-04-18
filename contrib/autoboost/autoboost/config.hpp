//  Boost config.hpp configuration header file  ------------------------------//

//  (C) Copyright John Maddock 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  CAUTION: This file is intended to be completely stable -
//           DO NOT MODIFY THIS FILE!
//

#ifndef AUTOBOOST_CONFIG_HPP
#define AUTOBOOST_CONFIG_HPP

// if we don't have a user config, then use the default location:
#if !defined(AUTOBOOST_USER_CONFIG) && !defined(AUTOBOOST_NO_USER_CONFIG)
#  define AUTOBOOST_USER_CONFIG <autoboost/config/user.hpp>
#if 0
// For dependency trackers:
#  include <autoboost/config/user.hpp>
#endif
#endif
// include it first:
#ifdef AUTOBOOST_USER_CONFIG
#  include AUTOBOOST_USER_CONFIG
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(AUTOBOOST_COMPILER_CONFIG) && !defined(AUTOBOOST_NO_COMPILER_CONFIG) && !defined(AUTOBOOST_NO_CONFIG)
#  include <autoboost/config/select_compiler_config.hpp>
#endif
// if we have a compiler config, include it now:
#ifdef AUTOBOOST_COMPILER_CONFIG
#  include AUTOBOOST_COMPILER_CONFIG
#endif

// if we don't have a std library config set, try and find one:
#if !defined(AUTOBOOST_STDLIB_CONFIG) && !defined(AUTOBOOST_NO_STDLIB_CONFIG) && !defined(AUTOBOOST_NO_CONFIG) && defined(__cplusplus)
#  include <autoboost/config/select_stdlib_config.hpp>
#endif
// if we have a std library config, include it now:
#ifdef AUTOBOOST_STDLIB_CONFIG
#  include AUTOBOOST_STDLIB_CONFIG
#endif

// if we don't have a platform config set, try and find one:
#if !defined(AUTOBOOST_PLATFORM_CONFIG) && !defined(AUTOBOOST_NO_PLATFORM_CONFIG) && !defined(AUTOBOOST_NO_CONFIG)
#  include <autoboost/config/select_platform_config.hpp>
#endif
// if we have a platform config, include it now:
#ifdef AUTOBOOST_PLATFORM_CONFIG
#  include AUTOBOOST_PLATFORM_CONFIG
#endif

// get config suffix code:
#include <autoboost/config/suffix.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#endif  // AUTOBOOST_CONFIG_HPP
