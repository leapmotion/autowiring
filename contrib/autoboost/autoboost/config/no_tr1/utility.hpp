//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// The aim of this header is just to include <utility> but to do
// so in a way that does not result in recursive inclusion of
// the Boost TR1 components if autoboost/tr1/tr1/utility is in the
// include search path.  We have to do this to avoid circular
// dependencies:
//

#ifndef AUTOBOOST_CONFIG_UTILITY
#  define AUTOBOOST_CONFIG_UTILITY

#  ifndef AUTOBOOST_TR1_NO_RECURSION
#     define AUTOBOOST_TR1_NO_RECURSION
#     define AUTOBOOST_CONFIG_NO_UTILITY_RECURSION
#  endif

#  include <utility>

#  ifdef AUTOBOOST_CONFIG_NO_UTILITY_RECURSION
#     undef AUTOBOOST_TR1_NO_RECURSION
#     undef AUTOBOOST_CONFIG_NO_UTILITY_RECURSION
#  endif

#endif
