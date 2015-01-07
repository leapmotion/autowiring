#ifndef AUTOBOOST_SERIALIZATION_CONFIG_HPP
#define AUTOBOOST_SERIALIZATION_CONFIG_HPP

//  config.hpp  ---------------------------------------------//

//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>
#include <autoboost/preprocessor/facilities/empty.hpp>

// note: this version incorporates the related code into the the 
// the same library as AUTOBOOST_ARCHIVE.  This could change some day in the
// future

// if AUTOBOOST_SERIALIZATION_DECL is defined undefine it now:
#ifdef AUTOBOOST_SERIALIZATION_DECL
    #undef AUTOBOOST_SERIALIZATION_DECL
#endif

#ifdef AUTOBOOST_HAS_DECLSPEC // defined in config system
// we need to import/export our code only if the user has specifically
// asked for it by defining either AUTOBOOST_ALL_DYN_LINK if they want all boost
// libraries to be dynamically linked, or AUTOBOOST_SERIALIZATION_DYN_LINK
// if they want just this one to be dynamically liked:
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SERIALIZATION_DYN_LINK)
    #if !defined(AUTOBOOST_DYN_LINK)
        #define AUTOBOOST_DYN_LINK
    #endif
    // export if this is our own source, otherwise import:
    #if defined(AUTOBOOST_SERIALIZATION_SOURCE)
        #if defined(__BORLANDC__)
            #define AUTOBOOST_SERIALIZATION_DECL(T) T __export
        #else
            #define AUTOBOOST_SERIALIZATION_DECL(T) __declspec(dllexport) T
        #endif
    #else
        #if defined(__BORLANDC__)
            #define AUTOBOOST_SERIALIZATION_DECL(T) T __import
        #else
            #define AUTOBOOST_SERIALIZATION_DECL(T) __declspec(dllimport) T
        #endif
    #endif // defined(AUTOBOOST_SERIALIZATION_SOURCE)
#endif // defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_SERIALIZATION_DYN_LINK)
#endif // AUTOBOOST_HAS_DECLSPEC

// if AUTOBOOST_SERIALIZATION_DECL isn't defined yet define it now:
#ifndef AUTOBOOST_SERIALIZATION_DECL
    #define AUTOBOOST_SERIALIZATION_DECL(T) T
#endif

#endif // AUTOBOOST_SERIALIZATION_CONFIG_HPP
