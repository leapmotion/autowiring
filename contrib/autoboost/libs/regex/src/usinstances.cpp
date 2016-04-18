/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE:        winstances.cpp
  *   VERSION:     see <autoboost/version.hpp>
  *   DESCRIPTION: regex unsigned short template instances (MSVC only).
  */

#define AUTOBOOST_REGEX_SOURCE
#ifdef _MSC_VER
#pragma warning(disable:4506) // 'no definition for inline function'
#endif

#include <autoboost/detail/workaround.hpp>
#include <memory>
#include <string>

#if defined(_DLL_CPPLIB) && !defined(_M_CEE_PURE) && defined(_NATIVE_WCHAR_T_DEFINED) \
   && !(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION) || defined(__STD_RWCOMPILER_H__) || defined(_RWSTD_VER))\
   && AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <1600)
//
// This is a horrible workaround, but without declaring these symbols extern we get
// duplicate symbol errors when linking if the application is built without
// /Zc:wchar_t
//
#ifdef _CRTIMP2_PURE
#  define AUTOBOOST_REGEX_STDLIB_DECL _CRTIMP2_PURE
#else
#  define AUTOBOOST_REGEX_STDLIB_DECL _CRTIMP2
#endif

namespace std{

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, >= 1400)
template class AUTOBOOST_REGEX_STDLIB_DECL allocator<unsigned short>;
template class AUTOBOOST_REGEX_STDLIB_DECL _String_val<unsigned short, allocator<unsigned short> >;
template class AUTOBOOST_REGEX_STDLIB_DECL basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >;
#endif

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, > 1300) && AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1400))
template<> AUTOBOOST_REGEX_STDLIB_DECL std::size_t __cdecl char_traits<unsigned short>::length(unsigned short const*);
#endif

template AUTOBOOST_REGEX_STDLIB_DECL bool __cdecl operator==(
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&,
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&);
template AUTOBOOST_REGEX_STDLIB_DECL bool __cdecl operator==(
   const unsigned short *,
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&);
template AUTOBOOST_REGEX_STDLIB_DECL bool __cdecl operator==(
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&,
   const unsigned short *);
template AUTOBOOST_REGEX_STDLIB_DECL bool __cdecl operator<(
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&,
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&);
template AUTOBOOST_REGEX_STDLIB_DECL bool __cdecl operator>(
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&,
   const basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >&);
}
#endif

#include <autoboost/regex/config.hpp>

#if !defined(AUTOBOOST_NO_WREGEX) && defined(AUTOBOOST_REGEX_HAS_OTHER_WCHAR_T) && !defined(AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES)
#define AUTOBOOST_REGEX_US_INSTANTIATE

#include <autoboost/regex.hpp>

#endif


