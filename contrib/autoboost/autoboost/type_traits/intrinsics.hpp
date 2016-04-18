//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_INTRINSICS_HPP_INCLUDED
#define AUTOBOOST_TT_INTRINSICS_HPP_INCLUDED

#ifndef AUTOBOOST_TT_CONFIG_HPP_INCLUDED
#include <autoboost/type_traits/config.hpp>
#endif

//
// Helper macros for builtin compiler support.
// If your compiler has builtin support for any of the following
// traits concepts, then redefine the appropriate macros to pick
// up on the compiler support:
//
// (these should largely ignore cv-qualifiers)
// AUTOBOOST_IS_UNION(T) should evaluate to true if T is a union type
// AUTOBOOST_IS_POD(T) should evaluate to true if T is a POD type
// AUTOBOOST_IS_EMPTY(T) should evaluate to true if T is an empty class type (and not a union)
// AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) should evaluate to true if "T x;" has no effect
// AUTOBOOST_HAS_TRIVIAL_COPY(T) should evaluate to true if T(t) <==> memcpy
// AUTOBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) should evaluate to true if T(autoboost::move(t)) <==> memcpy
// AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) should evaluate to true if t = u <==> memcpy
// AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) should evaluate to true if t = autoboost::move(u) <==> memcpy
// AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) should evaluate to true if ~T() has no effect
// AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) should evaluate to true if "T x;" can not throw
// AUTOBOOST_HAS_NOTHROW_COPY(T) should evaluate to true if T(t) can not throw
// AUTOBOOST_HAS_NOTHROW_ASSIGN(T) should evaluate to true if t = u can not throw
// AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) should evaluate to true T has a virtual destructor
//
// The following can also be defined: when detected our implementation is greatly simplified.
//
// AUTOBOOST_IS_ABSTRACT(T) true if T is an abstract type
// AUTOBOOST_IS_BASE_OF(T,U) true if T is a base class of U
// AUTOBOOST_IS_CLASS(T) true if T is a class type (and not a union)
// AUTOBOOST_IS_CONVERTIBLE(T,U) true if T is convertible to U
// AUTOBOOST_IS_ENUM(T) true is T is an enum
// AUTOBOOST_IS_POLYMORPHIC(T) true if T is a polymorphic type
// AUTOBOOST_ALIGNMENT_OF(T) should evaluate to the alignment requirements of type T.

#ifdef AUTOBOOST_HAS_SGI_TYPE_TRAITS
    // Hook into SGI's __type_traits class, this will pick up user supplied
    // specializations as well as SGI - compiler supplied specializations.
#   include <autoboost/type_traits/is_same.hpp>
#   ifdef __NetBSD__
      // There are two different versions of type_traits.h on NetBSD on Spark
      // use an implicit include via algorithm instead, to make sure we get
      // the same version as the std lib:
#     include <algorithm>
#   else
#    include <type_traits.h>
#   endif
#   define AUTOBOOST_IS_POD(T) ::autoboost::is_same< typename ::__type_traits<T>::is_POD_type, ::__true_type>::value
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) ::autoboost::is_same< typename ::__type_traits<T>::has_trivial_default_constructor, ::__true_type>::value
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) ::autoboost::is_same< typename ::__type_traits<T>::has_trivial_copy_constructor, ::__true_type>::value
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) ::autoboost::is_same< typename ::__type_traits<T>::has_trivial_assignment_operator, ::__true_type>::value
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) ::autoboost::is_same< typename ::__type_traits<T>::has_trivial_destructor, ::__true_type>::value

#   ifdef __sgi
#      define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#   endif
#endif

#if defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000)
    // Metrowerks compiler is acquiring intrinsic type traits support
    // post version 8.  We hook into the published interface to pick up
    // user defined specializations as well as compiler intrinsics as
    // and when they become available:
#   include <msl_utility>
#   define AUTOBOOST_IS_UNION(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::is_union<T>::value
#   define AUTOBOOST_IS_POD(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::is_POD<T>::value
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::has_trivial_default_ctor<T>::value
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::has_trivial_copy_ctor<T>::value
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::has_trivial_assignment<T>::value
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) AUTOBOOST_STD_EXTENSION_NAMESPACE::has_trivial_dtor<T>::value
#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if (defined(AUTOBOOST_MSVC) && defined(AUTOBOOST_MSVC_FULL_VER) && (AUTOBOOST_MSVC_FULL_VER >=140050215))\
         || (defined(AUTOBOOST_INTEL) && defined(_MSC_VER) && (_MSC_VER >= 1500))
#   include <autoboost/type_traits/is_same.hpp>
#   include <autoboost/type_traits/is_function.hpp>

#   define AUTOBOOST_IS_UNION(T) __is_union(T)
#   define AUTOBOOST_IS_POD(T) (__is_pod(T) && __has_trivial_constructor(T))
#   define AUTOBOOST_IS_EMPTY(T) __is_empty(T)
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T)|| ( ::autoboost::is_pod<T>::value && !::autoboost::is_volatile<T>::value))
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) || ( ::autoboost::is_pod<T>::value && ! ::autoboost::is_const<T>::value && !::autoboost::is_volatile<T>::value))
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T) || ::autoboost::is_pod<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_constructor(T) || ::autoboost::has_trivial_constructor<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) || ::autoboost::has_trivial_copy<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) || ::autoboost::has_trivial_assign<T>::value)
#   define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define AUTOBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define AUTOBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define AUTOBOOST_IS_CLASS(T) __is_class(T)
#   define AUTOBOOST_IS_CONVERTIBLE(T,U) ((__is_convertible_to(T,U) || (is_same<T,U>::value && !is_function<U>::value)) && !__is_abstract(U))
#   define AUTOBOOST_IS_ENUM(T) __is_enum(T)
//  This one doesn't quite always do the right thing:
//  #   define AUTOBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
//  This one fails if the default alignment has been changed with /Zp:
//  #   define AUTOBOOST_ALIGNMENT_OF(T) __alignof(T)

#   if defined(_MSC_VER) && (_MSC_VER >= 1700)
#       define AUTOBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) ((__has_trivial_move_constructor(T) || __is_pod(T)) && !::autoboost::is_volatile<T>::value && !::autoboost::is_reference<T>::value)
#       define AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) ((__has_trivial_move_assign(T) || __is_pod(T)) && ! ::autoboost::is_const<T>::value && !::autoboost::is_volatile<T>::value && !::autoboost::is_reference<T>::value)
#   endif

#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__DMC__) && (__DMC__ >= 0x848)
// For Digital Mars C++, www.digitalmars.com
#   define AUTOBOOST_IS_UNION(T) (__typeinfo(T) & 0x400)
#   define AUTOBOOST_IS_POD(T) (__typeinfo(T) & 0x800)
#   define AUTOBOOST_IS_EMPTY(T) (__typeinfo(T) & 0x1000)
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) (__typeinfo(T) & 0x10)
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) (__typeinfo(T) & 0x20)
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) (__typeinfo(T) & 0x40)
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__typeinfo(T) & 0x8)
#   define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__typeinfo(T) & 0x80)
#   define AUTOBOOST_HAS_NOTHROW_COPY(T) (__typeinfo(T) & 0x100)
#   define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) (__typeinfo(T) & 0x200)
#   define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) (__typeinfo(T) & 0x4)
#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(AUTOBOOST_CLANG) && defined(__has_feature)
#   include <cstddef>
#   include <autoboost/type_traits/is_same.hpp>
#   include <autoboost/type_traits/is_reference.hpp>
#   include <autoboost/type_traits/is_volatile.hpp>

#   if __has_feature(is_union)
#     define AUTOBOOST_IS_UNION(T) __is_union(T)
#   endif
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && __has_feature(is_pod)
#     define AUTOBOOST_IS_POD(T) __is_pod(T)
#   endif
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && __has_feature(is_empty)
#     define AUTOBOOST_IS_EMPTY(T) __is_empty(T)
#   endif
#   if __has_feature(has_trivial_constructor)
#     define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   endif
#   if __has_feature(has_trivial_copy)
#     define AUTOBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T) && !is_reference<T>::value && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_trivial_assign)
#     define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_trivial_destructor)
#     define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   endif
#   if __has_feature(has_nothrow_constructor)
#     define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   endif
#   if __has_feature(has_nothrow_copy)
#     define AUTOBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   endif
#   if __has_feature(has_nothrow_assign)
#     define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_virtual_destructor)
#     define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)
#   endif
#   if __has_feature(is_abstract)
#     define AUTOBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   endif
#   if __has_feature(is_base_of)
#     define AUTOBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   endif
#   if __has_feature(is_class)
#     define AUTOBOOST_IS_CLASS(T) __is_class(T)
#   endif
#   if __has_feature(is_convertible_to)
#     define AUTOBOOST_IS_CONVERTIBLE(T,U) __is_convertible_to(T,U)
#   endif
#   if __has_feature(is_enum)
#     define AUTOBOOST_IS_ENUM(T) __is_enum(T)
#   endif
#   if __has_feature(is_polymorphic)
#     define AUTOBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   endif
#   if __has_feature(has_trivial_move_constructor)
#     define AUTOBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) __has_trivial_move_constructor(T)
#   endif
#   if __has_feature(has_trivial_move_assign)
#     define AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) __has_trivial_move_assign(T)
#   endif
#   define AUTOBOOST_ALIGNMENT_OF(T) __alignof(T)
#   if __has_feature(is_final)
#     define AUTOBOOST_IS_FINAL(T) __is_final(T)
#   endif

#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3) && !defined(__GCCXML__))) && !defined(AUTOBOOST_CLANG)
#   include <autoboost/type_traits/is_same.hpp>
#   include <autoboost/type_traits/is_reference.hpp>
#   include <autoboost/type_traits/is_volatile.hpp>

#ifdef AUTOBOOST_INTEL
#  define AUTOBOOST_INTEL_TT_OPTS || is_pod<T>::value
#else
#  define AUTOBOOST_INTEL_TT_OPTS
#endif

#   define AUTOBOOST_IS_UNION(T) __is_union(T)
#   define AUTOBOOST_IS_POD(T) __is_pod(T)
#   define AUTOBOOST_IS_EMPTY(T) __is_empty(T)
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) ((__has_trivial_constructor(T) AUTOBOOST_INTEL_TT_OPTS) && ! ::autoboost::is_volatile<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) ((__has_trivial_copy(T) AUTOBOOST_INTEL_TT_OPTS) && !is_reference<T>::value && ! ::autoboost::is_volatile<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) ((__has_trivial_assign(T) AUTOBOOST_INTEL_TT_OPTS) && ! ::autoboost::is_volatile<T>::value && ! ::autoboost::is_const<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T) AUTOBOOST_INTEL_TT_OPTS)
#   define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_constructor(T) AUTOBOOST_INTEL_TT_OPTS)
#   define AUTOBOOST_HAS_NOTHROW_COPY(T) ((__has_nothrow_copy(T) AUTOBOOST_INTEL_TT_OPTS) && !is_volatile<T>::value && !is_reference<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) ((__has_nothrow_assign(T) AUTOBOOST_INTEL_TT_OPTS) && !is_volatile<T>::value && !is_const<T>::value)
#   define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define AUTOBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define AUTOBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define AUTOBOOST_IS_CLASS(T) __is_class(T)
#   define AUTOBOOST_IS_ENUM(T) __is_enum(T)
#   define AUTOBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   if (!defined(unix) && !defined(__unix__)) || defined(__LP64__)
      // GCC sometimes lies about alignment requirements
      // of type double on 32-bit unix platforms, use the
      // old implementation instead in that case:
#     define AUTOBOOST_ALIGNMENT_OF(T) __alignof__(T)
#   endif
#   if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))
#     define AUTOBOOST_IS_FINAL(T) __is_final(T)
#   endif

#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600)
#   include <autoboost/type_traits/is_same.hpp>
#   include <autoboost/type_traits/is_reference.hpp>
#   include <autoboost/type_traits/is_volatile.hpp>

#   define AUTOBOOST_IS_UNION(T) __is_union(T)
#   define AUTOBOOST_IS_POD(T) __is_pod(T)
#   define AUTOBOOST_IS_EMPTY(T) __is_empty(T)
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T) && !is_reference<T>::value && !is_volatile<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   define AUTOBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define AUTOBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define AUTOBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define AUTOBOOST_IS_CLASS(T) __is_class(T)
#   define AUTOBOOST_IS_ENUM(T) __is_enum(T)
#   define AUTOBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   define AUTOBOOST_ALIGNMENT_OF(T) __alignof__(T)
#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

# if defined(__CODEGEARC__)
#   include <autoboost/type_traits/is_same.hpp>
#   include <autoboost/type_traits/is_reference.hpp>
#   include <autoboost/type_traits/is_volatile.hpp>
#   include <autoboost/type_traits/is_void.hpp>

#   define AUTOBOOST_IS_UNION(T) __is_union(T)
#   define AUTOBOOST_IS_POD(T) __is_pod(T)
#   define AUTOBOOST_IS_EMPTY(T) __is_empty(T)
#   define AUTOBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) (__has_trivial_default_constructor(T))
#   define AUTOBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy_constructor(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   define AUTOBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T))
#   define AUTOBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_default_constructor(T))
#   define AUTOBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy_constructor(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define AUTOBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   define AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define AUTOBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define AUTOBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_void<T>::value && !is_void<U>::value)
#   define AUTOBOOST_IS_CLASS(T) __is_class(T)
#   define AUTOBOOST_IS_CONVERTIBLE(T,U) (__is_convertible(T,U) || is_void<U>::value)
#   define AUTOBOOST_IS_ENUM(T) __is_enum(T)
#   define AUTOBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   define AUTOBOOST_ALIGNMENT_OF(T) alignof(T)

#   define AUTOBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#endif // AUTOBOOST_TT_INTRINSICS_HPP_INCLUDED







