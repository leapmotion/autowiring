#ifndef AUTOBOOST_ARCHIVE_DETAIL_DECL_HPP
#define AUTOBOOST_ARCHIVE_DETAIL_DECL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  decl.hpp
//
//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------//

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <autoboost/config.hpp>

#if (defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SERIALIZATION_DYN_LINK))
    #if defined(AUTOBOOST_ARCHIVE_SOURCE)
        #define AUTOBOOST_ARCHIVE_DECL AUTOBOOST_SYMBOL_EXPORT
    #else
        #define AUTOBOOST_ARCHIVE_DECL AUTOBOOST_SYMBOL_IMPORT
    #endif
    #if defined(AUTOBOOST_WARCHIVE_SOURCE)
        #define AUTOBOOST_WARCHIVE_DECL AUTOBOOST_SYMBOL_EXPORT
    #else
        #define AUTOBOOST_WARCHIVE_DECL AUTOBOOST_SYMBOL_IMPORT
    #endif
    #if !defined(AUTOBOOST_WARCHIVE_SOURCE) && !defined(AUTOBOOST_ARCHIVE_SOURCE)
        #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL AUTOBOOST_SYMBOL_IMPORT
    #endif

    #if defined(AUTOBOOST_WARCHIVE_SOURCE) || defined(AUTOBOOST_ARCHIVE_SOURCE)
        #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL AUTOBOOST_SYMBOL_EXPORT
    #else
        #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL AUTOBOOST_SYMBOL_IMPORT
    #endif

#endif

#if ! defined(AUTOBOOST_ARCHIVE_DECL)
    #define AUTOBOOST_ARCHIVE_DECL
#endif
#if ! defined(AUTOBOOST_WARCHIVE_DECL)
    #define AUTOBOOST_WARCHIVE_DECL
#endif
#if ! defined(AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL)
    #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
#endif

#endif // AUTOBOOST_ARCHIVE_DETAIL_DECL_HPP
