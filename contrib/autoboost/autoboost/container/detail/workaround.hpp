//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_WORKAROUND_HPP
#define AUTOBOOST_CONTAINER_DETAIL_WORKAROUND_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#if    !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)\
    && !defined(AUTOBOOST_INTERPROCESS_DISABLE_VARIADIC_TMPL)
   #define AUTOBOOST_CONTAINER_PERFECT_FORWARDING
#endif

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && defined(__GXX_EXPERIMENTAL_CXX0X__)\
    && (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__ < 40700)
   #define AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST
#endif

#if !defined(AUTOBOOST_FALLTHOUGH)
   #define AUTOBOOST_CONTAINER_FALLTHOUGH
#else
   #define AUTOBOOST_CONTAINER_FALLTHOUGH AUTOBOOST_FALLTHOUGH;
#endif

//Macros for documentation purposes. For code, expands to the argument
#define AUTOBOOST_CONTAINER_IMPDEF(TYPE) TYPE
#define AUTOBOOST_CONTAINER_SEEDOC(TYPE) TYPE

//Macros for memset optimization. In most platforms
//memsetting pointers and floatings is safe and faster.
//
//If your platform does not offer these guarantees
//define these to value zero.
#ifndef AUTOBOOST_CONTAINER_MEMZEROED_FLOATING_POINT_IS_NOT_ZERO
#define AUTOBOOST_CONTAINER_MEMZEROED_FLOATING_POINT_IS_ZERO 1
#endif

#ifndef AUTOBOOST_CONTAINER_MEMZEROED_POINTER_IS_NOT_NULL
#define AUTOBOOST_CONTAINER_MEMZEROED_POINTER_IS_NULL
#endif

#define AUTOBOOST_CONTAINER_DOC1ST(TYPE1, TYPE2) TYPE2
#define AUTOBOOST_CONTAINER_I ,
#define AUTOBOOST_CONTAINER_DOCIGN(T) T
#define AUTOBOOST_CONTAINER_DOCONLY(T)

/*
   we need to import/export our code only if the user has specifically
   asked for it by defining either AUTOBOOST_ALL_DYN_LINK if they want all autoboost
   libraries to be dynamically linked, or AUTOBOOST_CONTAINER_DYN_LINK
   if they want just this one to be dynamically liked:
*/
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_CONTAINER_DYN_LINK)

   /* export if this is our own source, otherwise import: */
   #ifdef AUTOBOOST_CONTAINER_SOURCE
   #  define AUTOBOOST_CONTAINER_DECL AUTOBOOST_SYMBOL_EXPORT
   #else
   #  define AUTOBOOST_CONTAINER_DECL AUTOBOOST_SYMBOL_IMPORT

   #endif  /* AUTOBOOST_CONTAINER_SOURCE */
#else
   #define AUTOBOOST_CONTAINER_DECL
#endif  /* DYN_LINK */

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_WORKAROUND_HPP
