//  autoboost/system/config.hpp  -----------------------------------------------------------//

//  Copyright Beman Dawes 2003, 2006

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/system for documentation.

#ifndef AUTOBOOST_SYSTEM_CONFIG_HPP
#define AUTOBOOST_SYSTEM_CONFIG_HPP

#include <autoboost/config.hpp>
#include <autoboost/predef/platform.h>
#include <autoboost/system/api_config.hpp>  // for AUTOBOOST_POSIX_API or AUTOBOOST_WINDOWS_API

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(AUTOBOOST_SYSTEM_DYN_LINK) && !defined(AUTOBOOST_SYSTEM_STATIC_LINK) \
  && !defined(AUTOBOOST_ALL_DYN_LINK) && !defined(AUTOBOOST_ALL_STATIC_LINK)
# define AUTOBOOST_SYSTEM_STATIC_LINK
#endif

#if defined(AUTOBOOST_ALL_DYN_LINK) && !defined(AUTOBOOST_SYSTEM_DYN_LINK)
# define AUTOBOOST_SYSTEM_DYN_LINK
#elif defined(AUTOBOOST_ALL_STATIC_LINK) && !defined(AUTOBOOST_SYSTEM_STATIC_LINK)
# define AUTOBOOST_SYSTEM_STATIC_LINK
#endif

#if defined(AUTOBOOST_SYSTEM_DYN_LINK) && defined(AUTOBOOST_SYSTEM_STATIC_LINK)
# error Must not define both AUTOBOOST_SYSTEM_DYN_LINK and AUTOBOOST_SYSTEM_STATIC_LINK
#endif

//  enable dynamic or static linking as requested --------------------------------------//

#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SYSTEM_DYN_LINK)
# if defined(AUTOBOOST_SYSTEM_SOURCE)
#   define AUTOBOOST_SYSTEM_DECL AUTOBOOST_SYMBOL_EXPORT
# else
#   define AUTOBOOST_SYSTEM_DECL AUTOBOOST_SYMBOL_IMPORT
# endif
#else
# define AUTOBOOST_SYSTEM_DECL
#endif

#endif // AUTOBOOST_SYSTEM_CONFIG_HPP

