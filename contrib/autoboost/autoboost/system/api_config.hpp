//  autoboost/system/api_config.hpp  -------------------------------------------------------//

//  Copyright Beman Dawes 2003, 2006, 2010

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/system for documentation.

//--------------------------------------------------------------------------------------//

//  Boost.System calls operating system API functions to implement system error category
//  functions. Usually there is no question as to which API is to be used.
//
//  In the case of MinGW or Cygwin/MinGW, however, both POSIX and Windows API's are
//  available. Chaos ensues if other code thinks one is in use when Boost.System was
//  actually built with the other. This header centralizes the API choice and prevents
//  user definition of API macros, thus elminating the possibility of mismatches and the
//  need to test configurations with little or no practical value.
//

//--------------------------------------------------------------------------------------//

#ifndef AUTOBOOST_SYSTEM_API_CONFIG_HPP
#define AUTOBOOST_SYSTEM_API_CONFIG_HPP

# if defined(AUTOBOOST_POSIX_API) || defined(AUTOBOOST_WINDOWS_API)
#   error user defined AUTOBOOST_POSIX_API or AUTOBOOST_WINDOWS_API not supported
# endif

//  AUTOBOOST_POSIX_API or AUTOBOOST_WINDOWS_API specify which API to use
//    Cygwin/MinGW does not predefine _WIN32.
//    Standalone MinGW and all other known Windows compilers do predefine _WIN32
//    Compilers that predefine _WIN32 or __MINGW32__ do so for Windows 64-bit builds too.

# if defined(_WIN32) || defined(__CYGWIN__) // Windows default, including MinGW and Cygwin
#   define AUTOBOOST_WINDOWS_API
# else
#   define AUTOBOOST_POSIX_API
# endif

#endif  // AUTOBOOST_SYSTEM_API_CONFIG_HPP
