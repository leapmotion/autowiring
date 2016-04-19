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

//  enable automatic library variant selection  ----------------------------------------//

#if !defined(AUTOBOOST_SYSTEM_SOURCE) && !defined(AUTOBOOST_ALL_NO_LIB) && !defined(AUTOBOOST_SYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define AUTOBOOST_LIB_NAME autoboost_system
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SYSTEM_DYN_LINK)
#  define AUTOBOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <autoboost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // AUTOBOOST_SYSTEM_CONFIG_HPP

