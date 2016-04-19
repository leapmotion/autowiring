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
  *   FILE         iterator_traits.cpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Declares iterator traits workarounds.
  */

#ifndef AUTOBOOST_REGEX_V4_ITERATOR_TRAITS_HPP
#define AUTOBOOST_REGEX_V4_ITERATOR_TRAITS_HPP

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

namespace autoboost{
namespace AUTOBOOST_REGEX_DETAIL_NS{

#if defined(AUTOBOOST_NO_STD_ITERATOR_TRAITS)

template <class T>
struct regex_iterator_traits
{
  typedef typename T::iterator_category iterator_category;
  typedef typename T::value_type        value_type;
#if !defined(AUTOBOOST_NO_STD_ITERATOR)
  typedef typename T::difference_type   difference_type;
  typedef typename T::pointer           pointer;
  typedef typename T::reference         reference;
#else
  typedef std::ptrdiff_t                difference_type;
  typedef value_type*                   pointer;
  typedef value_type&                   reference;
#endif
};

template <class T>
struct pointer_iterator_traits
{
   typedef std::ptrdiff_t difference_type;
   typedef T value_type;
   typedef T* pointer;
   typedef T& reference;
   typedef std::random_access_iterator_tag iterator_category;
};
template <class T>
struct const_pointer_iterator_traits
{
   typedef std::ptrdiff_t difference_type;
   typedef T value_type;
   typedef const T* pointer;
   typedef const T& reference;
   typedef std::random_access_iterator_tag iterator_category;
};

template<>
struct regex_iterator_traits<char*> : pointer_iterator_traits<char>{};
template<>
struct regex_iterator_traits<const char*> : const_pointer_iterator_traits<char>{};
template<>
struct regex_iterator_traits<wchar_t*> : pointer_iterator_traits<wchar_t>{};
template<>
struct regex_iterator_traits<const wchar_t*> : const_pointer_iterator_traits<wchar_t>{};
//
// the follwoing are needed for ICU support:
//
template<>
struct regex_iterator_traits<unsigned char*> : pointer_iterator_traits<char>{};
template<>
struct regex_iterator_traits<const unsigned char*> : const_pointer_iterator_traits<char>{};
template<>
struct regex_iterator_traits<int*> : pointer_iterator_traits<int>{};
template<>
struct regex_iterator_traits<const int*> : const_pointer_iterator_traits<int>{};

#ifdef AUTOBOOST_REGEX_HAS_OTHER_WCHAR_T
template<>
struct regex_iterator_traits<unsigned short*> : pointer_iterator_traits<unsigned short>{};
template<>
struct regex_iterator_traits<const unsigned short*> : const_pointer_iterator_traits<unsigned short>{};
#endif

#if defined(__SGI_STL_PORT) && defined(__STL_DEBUG)
template<>
struct regex_iterator_traits<std::string::iterator> : pointer_iterator_traits<char>{};
template<>
struct regex_iterator_traits<std::string::const_iterator> : const_pointer_iterator_traits<char>{};
#ifndef AUTOBOOST_NO_STD_WSTRING
template<>
struct regex_iterator_traits<std::wstring::iterator> : pointer_iterator_traits<wchar_t>{};
template<>
struct regex_iterator_traits<std::wstring::const_iterator> : const_pointer_iterator_traits<wchar_t>{};
#endif // AUTOBOOST_NO_WSTRING
#endif // stport

#else

template <class T>
struct regex_iterator_traits : public std::iterator_traits<T> {};

#endif

} // namespace AUTOBOOST_REGEX_DETAIL_NS
} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#endif

