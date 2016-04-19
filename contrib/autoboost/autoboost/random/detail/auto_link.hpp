/* autoboost random auto_link.hpp header file
 *
 * Copyright Steven Watanabe 2010
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * $Id$
 */

#ifndef AUTOBOOST_RANDOM_DETAIL_AUTO_LINK_HPP
#define AUTOBOOST_RANDOM_DETAIL_AUTO_LINK_HPP

#include <autoboost/config.hpp>

#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_RANDOM_DYN_LINK)
    #if defined(AUTOBOOST_RANDOM_SOURCE)
        #define AUTOBOOST_RANDOM_DECL AUTOBOOST_SYMBOL_EXPORT
    #else
        #define AUTOBOOST_RANDOM_DECL AUTOBOOST_SYMBOL_IMPORT
    #endif
#endif

#ifndef AUTOBOOST_RANDOM_DECL
    #define AUTOBOOST_RANDOM_DECL
#endif

#if !defined(AUTOBOOST_RANDOM_NO_LIB) && !defined(AUTOBOOST_ALL_NO_LIB) && !defined(AUTOBOOST_RANDOM_SOURCE)

#define AUTOBOOST_LIB_NAME autoboost_random

#if defined(AUTOBOOST_RANDOM_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
    #define AUTOBOOST_DYN_LINK
#endif

#include <autoboost/config/auto_link.hpp>

#endif

#endif
