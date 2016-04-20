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
#include <autoboost/preprocessor/facilities/empty.hpp>

#if defined(AUTOBOOST_HAS_DECLSPEC)
    #if (defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SERIALIZATION_DYN_LINK))
        #if defined(AUTOBOOST_ARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define AUTOBOOST_ARCHIVE_DECL(T) T __export
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T)  T __export
            #else
            #define AUTOBOOST_ARCHIVE_DECL(T) __declspec(dllexport) T
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T)  __declspec(dllexport) T
            #endif
        #else
            #if defined(__BORLANDC__)
            #define AUTOBOOST_ARCHIVE_DECL(T) T __import
            #else
            #define AUTOBOOST_ARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
        #if defined(AUTOBOOST_WARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define AUTOBOOST_WARCHIVE_DECL(T) T __export
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T) T __export
            #else
            #define AUTOBOOST_WARCHIVE_DECL(T) __declspec(dllexport) T
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T) __declspec(dllexport) T
            #endif
        #else
            #if defined(__BORLANDC__)
            #define AUTOBOOST_WARCHIVE_DECL(T) T __import
            #else
            #define AUTOBOOST_WARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
        #if !defined(AUTOBOOST_WARCHIVE_SOURCE) && !defined(AUTOBOOST_ARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T) T __import
            #else
            #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
    #endif
#endif // AUTOBOOST_HAS_DECLSPEC

#if ! defined(AUTOBOOST_ARCHIVE_DECL)
    #define AUTOBOOST_ARCHIVE_DECL(T) T
#endif
#if ! defined(AUTOBOOST_WARCHIVE_DECL)
    #define AUTOBOOST_WARCHIVE_DECL(T) T
#endif
#if ! defined(AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL)
    #define AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(T) T
#endif

#endif // AUTOBOOST_ARCHIVE_DETAIL_DECL_HPP
