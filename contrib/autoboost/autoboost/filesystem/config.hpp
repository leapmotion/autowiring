//  autoboost/filesystem/v3/config.hpp  ----------------------------------------------------//

//  Copyright Beman Dawes 2003

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef AUTOBOOST_FILESYSTEM3_CONFIG_HPP
#define AUTOBOOST_FILESYSTEM3_CONFIG_HPP

# if defined(AUTOBOOST_FILESYSTEM_VERSION) && AUTOBOOST_FILESYSTEM_VERSION != 3
#   error Compiling Filesystem version 3 file with AUTOBOOST_FILESYSTEM_VERSION defined != 3
# endif

# if !defined(AUTOBOOST_FILESYSTEM_VERSION)
#   define AUTOBOOST_FILESYSTEM_VERSION 3
# endif

#define AUTOBOOST_FILESYSTEM_I18N  // aid users wishing to compile several versions

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <autoboost/config.hpp>
#include <autoboost/system/api_config.hpp>  // for AUTOBOOST_POSIX_API or AUTOBOOST_WINDOWS_API
#include <autoboost/detail/workaround.hpp>

//  AUTOBOOST_FILESYSTEM_DEPRECATED needed for source compiles -----------------------------//

# ifdef AUTOBOOST_FILESYSTEM_SOURCE
#   define AUTOBOOST_FILESYSTEM_DEPRECATED
#   undef AUTOBOOST_FILESYSTEM_NO_DEPRECATED   // fixes #9454, src bld fails if NO_DEP defined
# endif

//  throw an exception  ----------------------------------------------------------------//
//
//  Exceptions were originally thrown via autoboost::throw_exception().
//  As throw_exception() became more complex, it caused user error reporting
//  to be harder to interpret, since the exception reported became much more complex.
//  The immediate fix was to throw directly, wrapped in a macro to make any later change
//  easier.

#define AUTOBOOST_FILESYSTEM_THROW(EX) throw EX

# if defined( AUTOBOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

//  This header implements separate compilation features as described in
//  http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(AUTOBOOST_FILESYSTEM_DYN_LINK) && !defined(AUTOBOOST_FILESYSTEM_STATIC_LINK) \
  && !defined(AUTOBOOST_ALL_DYN_LINK) && !defined(AUTOBOOST_ALL_STATIC_LINK)
# define AUTOBOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(AUTOBOOST_ALL_DYN_LINK) && !defined(AUTOBOOST_FILESYSTEM_DYN_LINK)
# define AUTOBOOST_FILESYSTEM_DYN_LINK
#elif defined(AUTOBOOST_ALL_STATIC_LINK) && !defined(AUTOBOOST_FILESYSTEM_STATIC_LINK)
# define AUTOBOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(AUTOBOOST_FILESYSTEM_DYN_LINK) && defined(AUTOBOOST_FILESYSTEM_STATIC_LINK)
# error Must not define both AUTOBOOST_FILESYSTEM_DYN_LINK and AUTOBOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(AUTOBOOST_ALL_NO_LIB) && !defined(AUTOBOOST_FILESYSTEM_NO_LIB)
# define AUTOBOOST_FILESYSTEM_NO_LIB
#endif

//  enable dynamic linking  ------------------------------------------------------------//

#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_FILESYSTEM_DYN_LINK)
# if defined(AUTOBOOST_FILESYSTEM_SOURCE)
#   define AUTOBOOST_FILESYSTEM_DECL AUTOBOOST_SYMBOL_EXPORT
# else
#   define AUTOBOOST_FILESYSTEM_DECL AUTOBOOST_SYMBOL_IMPORT
# endif
#else
# define AUTOBOOST_FILESYSTEM_DECL
#endif

//  enable automatic library variant selection  ----------------------------------------//

#if !defined(AUTOBOOST_FILESYSTEM_SOURCE) && !defined(AUTOBOOST_ALL_NO_LIB) \
  && !defined(AUTOBOOST_FILESYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define AUTOBOOST_LIB_NAME autoboost_filesystem
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_FILESYSTEM_DYN_LINK)
#  define AUTOBOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <autoboost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // AUTOBOOST_FILESYSTEM3_CONFIG_HPP
