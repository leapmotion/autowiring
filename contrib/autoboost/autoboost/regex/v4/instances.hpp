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
  *   FILE         instances.cpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Defines those template instances that are placed in the
  *                library rather than in the users object files.
  */

//
// note no include guard, we may include this multiple times:
//
#ifndef AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES

namespace autoboost{

//
// this header can be included multiple times, each time with
// a different character type, AUTOBOOST_REGEX_CHAR_T must be defined
// first:
//
#ifndef AUTOBOOST_REGEX_CHAR_T
#  error "AUTOBOOST_REGEX_CHAR_T not defined"
#endif

#ifndef AUTOBOOST_REGEX_TRAITS_T
#  define AUTOBOOST_REGEX_TRAITS_T , autoboost::regex_traits<AUTOBOOST_REGEX_CHAR_T >
#endif

//
// what follows is compiler specific:
//

#if  defined(__BORLANDC__) && (__BORLANDC__ < 0x600)

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

#  ifndef AUTOBOOST_REGEX_INSTANTIATE
#     pragma option push -Jgx
#  endif

template class AUTOBOOST_REGEX_DECL basic_regex< AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >;
template class AUTOBOOST_REGEX_DECL match_results< const AUTOBOOST_REGEX_CHAR_T* >;
#ifndef AUTOBOOST_NO_STD_ALLOCATOR
template class AUTOBOOST_REGEX_DECL ::autoboost::re_detail::perl_matcher<AUTOBOOST_REGEX_CHAR_T const *, match_results< const AUTOBOOST_REGEX_CHAR_T* >::allocator_type AUTOBOOST_REGEX_TRAITS_T >;
#endif

#  ifndef AUTOBOOST_REGEX_INSTANTIATE
#     pragma option pop
#  endif

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

#elif defined(AUTOBOOST_MSVC) || defined(__ICL)

#  ifndef AUTOBOOST_REGEX_INSTANTIATE
#     ifdef __GNUC__
#        define template __extension__ extern template
#     else
#        if AUTOBOOST_MSVC > 1310
#           define AUTOBOOST_REGEX_TEMPLATE_DECL
#        endif
#        define template extern template
#     endif
#  endif

#ifndef AUTOBOOST_REGEX_TEMPLATE_DECL
#  define AUTOBOOST_REGEX_TEMPLATE_DECL AUTOBOOST_REGEX_DECL
#endif

#  ifdef AUTOBOOST_MSVC
#     pragma warning(push)
#     pragma warning(disable : 4251 4231)
#     if AUTOBOOST_MSVC < 1600
#     pragma warning(disable : 4660)
#     endif
#  endif

template class AUTOBOOST_REGEX_TEMPLATE_DECL basic_regex< AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >;

template class AUTOBOOST_REGEX_TEMPLATE_DECL match_results< const AUTOBOOST_REGEX_CHAR_T* >;
#ifndef AUTOBOOST_NO_STD_ALLOCATOR
template class AUTOBOOST_REGEX_TEMPLATE_DECL ::autoboost::re_detail::perl_matcher<AUTOBOOST_REGEX_CHAR_T const *, match_results< const AUTOBOOST_REGEX_CHAR_T* >::allocator_type AUTOBOOST_REGEX_TRAITS_T >;
#endif
#if !(defined(AUTOBOOST_DINKUMWARE_STDLIB) && (AUTOBOOST_DINKUMWARE_STDLIB <= 1))\
   && !(defined(AUTOBOOST_INTEL_CXX_VERSION) && (AUTOBOOST_INTEL_CXX_VERSION <= 800))\
   && !(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))\
   && !defined(AUTOBOOST_REGEX_ICU_INSTANCES)
template class AUTOBOOST_REGEX_TEMPLATE_DECL match_results< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator >;
#ifndef AUTOBOOST_NO_STD_ALLOCATOR
template class AUTOBOOST_REGEX_TEMPLATE_DECL ::autoboost::re_detail::perl_matcher< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, autoboost::regex_traits<AUTOBOOST_REGEX_CHAR_T > >;
#endif
#endif


#  ifdef AUTOBOOST_MSVC
#     pragma warning(pop)
#  endif

#  ifdef template
#     undef template
#  endif

#undef AUTOBOOST_REGEX_TEMPLATE_DECL

#elif (defined(__GNUC__) && (__GNUC__ >= 3)) || !defined(AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE)

#  ifndef AUTOBOOST_REGEX_INSTANTIATE
#     ifdef __GNUC__
#        define template __extension__ extern template
#     else
#        define template extern template
#     endif
#  endif

#if !defined(AUTOBOOST_NO_STD_LOCALE) && !defined(AUTOBOOST_REGEX_ICU_INSTANCES)
namespace re_detail{
template AUTOBOOST_REGEX_DECL
std::locale cpp_regex_traits_base<AUTOBOOST_REGEX_CHAR_T>::imbue(const std::locale& l);

template AUTOBOOST_REGEX_DECL
cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::string_type
   cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::transform_primary(const AUTOBOOST_REGEX_CHAR_T* p1, const AUTOBOOST_REGEX_CHAR_T* p2) const;
template AUTOBOOST_REGEX_DECL
cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::string_type
   cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::transform(const AUTOBOOST_REGEX_CHAR_T* p1, const AUTOBOOST_REGEX_CHAR_T* p2) const;
template AUTOBOOST_REGEX_DECL
cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::string_type
   cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::lookup_collatename(const AUTOBOOST_REGEX_CHAR_T* p1, const AUTOBOOST_REGEX_CHAR_T* p2) const;
template AUTOBOOST_REGEX_DECL
void cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::init();
template AUTOBOOST_REGEX_DECL
cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::char_class_type
   cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::lookup_classname_imp(const AUTOBOOST_REGEX_CHAR_T* p1, const AUTOBOOST_REGEX_CHAR_T* p2) const;
#ifdef AUTOBOOST_REGEX_BUGGY_CTYPE_FACET
template AUTOBOOST_REGEX_DECL
bool cpp_regex_traits_implementation<AUTOBOOST_REGEX_CHAR_T>::isctype(const AUTOBOOST_REGEX_CHAR_T c, char_class_type mask) const;
#endif
} // namespace
template AUTOBOOST_REGEX_DECL
int cpp_regex_traits<AUTOBOOST_REGEX_CHAR_T>::toi(const AUTOBOOST_REGEX_CHAR_T*& first, const AUTOBOOST_REGEX_CHAR_T* last, int radix)const;
template AUTOBOOST_REGEX_DECL
std::string cpp_regex_traits<AUTOBOOST_REGEX_CHAR_T>::catalog_name(const std::string& name);
template AUTOBOOST_REGEX_DECL
std::string& cpp_regex_traits<AUTOBOOST_REGEX_CHAR_T>::get_catalog_name_inst();
template AUTOBOOST_REGEX_DECL
std::string cpp_regex_traits<AUTOBOOST_REGEX_CHAR_T>::get_catalog_name();
#ifdef AUTOBOOST_HAS_THREADS
template AUTOBOOST_REGEX_DECL
static_mutex& cpp_regex_traits<AUTOBOOST_REGEX_CHAR_T>::get_mutex_inst();
#endif
#endif

template AUTOBOOST_REGEX_DECL basic_regex<AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >&
   basic_regex<AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >::do_assign(
      const AUTOBOOST_REGEX_CHAR_T* p1,
      const AUTOBOOST_REGEX_CHAR_T* p2,
      flag_type f);
template AUTOBOOST_REGEX_DECL basic_regex<AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >::locale_type AUTOBOOST_REGEX_CALL
   basic_regex<AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >::imbue(locale_type l);

template AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL
   match_results<const AUTOBOOST_REGEX_CHAR_T*>::maybe_assign(
      const match_results<const AUTOBOOST_REGEX_CHAR_T*>& m);

namespace re_detail{
template AUTOBOOST_REGEX_DECL void perl_matcher<AUTOBOOST_REGEX_CHAR_T const *, match_results< const AUTOBOOST_REGEX_CHAR_T* >::allocator_type AUTOBOOST_REGEX_TRAITS_T >::construct_init(
      const basic_regex<AUTOBOOST_REGEX_CHAR_T AUTOBOOST_REGEX_TRAITS_T >& e, match_flag_type f);
template AUTOBOOST_REGEX_DECL bool perl_matcher<AUTOBOOST_REGEX_CHAR_T const *, match_results< const AUTOBOOST_REGEX_CHAR_T* >::allocator_type AUTOBOOST_REGEX_TRAITS_T >::match();
template AUTOBOOST_REGEX_DECL bool perl_matcher<AUTOBOOST_REGEX_CHAR_T const *, match_results< const AUTOBOOST_REGEX_CHAR_T* >::allocator_type AUTOBOOST_REGEX_TRAITS_T >::find();
} // namespace

#if (defined(__GLIBCPP__) || defined(__GLIBCXX__)) \
   && !defined(AUTOBOOST_REGEX_ICU_INSTANCES)\
   && !defined(__SGI_STL_PORT)\
   && !defined(_STLPORT_VERSION)
// std:basic_string<>::const_iterator instances as well:
template AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL
   match_results<std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator>::maybe_assign(
      const match_results<std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator>& m);

namespace re_detail{
template AUTOBOOST_REGEX_DECL void perl_matcher<std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, autoboost::regex_traits<AUTOBOOST_REGEX_CHAR_T > >::construct_init(
      const basic_regex<AUTOBOOST_REGEX_CHAR_T>& e, match_flag_type f);
template AUTOBOOST_REGEX_DECL bool perl_matcher<std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, autoboost::regex_traits<AUTOBOOST_REGEX_CHAR_T > >::match();
template AUTOBOOST_REGEX_DECL bool perl_matcher<std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<AUTOBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, autoboost::regex_traits<AUTOBOOST_REGEX_CHAR_T > >::find();
} // namespace
#endif

#  ifdef template
#     undef template
#  endif


#endif

} // namespace autoboost

#endif // AUTOBOOST_REGEX_NO_EXTERNAL_TEMPLATES





