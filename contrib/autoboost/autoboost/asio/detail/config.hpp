//
// detail/config.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_CONFIG_HPP
#define AUTOBOOST_ASIO_DETAIL_CONFIG_HPP

#if defined(AUTOBOOST_ASIO_STANDALONE)
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ARRAY 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ASSERT 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_BIND 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_CHRONO 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_DATE_TIME 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_LIMITS 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_REGEX 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_STATIC_CONSTANT 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_THROW_EXCEPTION 1
# define AUTOBOOST_ASIO_DISABLE_AUTOBOOST_WORKAROUND 1
#else // defined(AUTOBOOST_ASIO_STANDALONE)
# include <autoboost/config.hpp>
# include <autoboost/version.hpp>
# define AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG 1
#endif // defined(AUTOBOOST_ASIO_STANDALONE)

// Default to a header-only implementation. The user must specifically request
// separate compilation by defining either AUTOBOOST_ASIO_SEPARATE_COMPILATION or
// AUTOBOOST_ASIO_DYN_LINK (as a DLL/shared library implies separate compilation).
#if !defined(AUTOBOOST_ASIO_HEADER_ONLY)
# if !defined(AUTOBOOST_ASIO_SEPARATE_COMPILATION)
#  if !defined(AUTOBOOST_ASIO_DYN_LINK)
#   define AUTOBOOST_ASIO_HEADER_ONLY 1
#  endif // !defined(AUTOBOOST_ASIO_DYN_LINK)
# endif // !defined(AUTOBOOST_ASIO_SEPARATE_COMPILATION)
#endif // !defined(AUTOBOOST_ASIO_HEADER_ONLY)

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# define AUTOBOOST_ASIO_DECL inline
#else // defined(AUTOBOOST_ASIO_HEADER_ONLY)
# if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
// We need to import/export our code only if the user has specifically asked
// for it by defining AUTOBOOST_ASIO_DYN_LINK.
#  if defined(AUTOBOOST_ASIO_DYN_LINK)
// Export if this is our own source, otherwise import.
#   if defined(AUTOBOOST_ASIO_SOURCE)
#    define AUTOBOOST_ASIO_DECL __declspec(dllexport)
#   else // defined(AUTOBOOST_ASIO_SOURCE)
#    define AUTOBOOST_ASIO_DECL __declspec(dllimport)
#   endif // defined(AUTOBOOST_ASIO_SOURCE)
#  endif // defined(AUTOBOOST_ASIO_DYN_LINK)
# endif // defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

// If AUTOBOOST_ASIO_DECL isn't defined yet define it now.
#if !defined(AUTOBOOST_ASIO_DECL)
# define AUTOBOOST_ASIO_DECL
#endif // !defined(AUTOBOOST_ASIO_DECL)

// Microsoft Visual C++ detection.
#if !defined(AUTOBOOST_ASIO_MSVC)
# if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_MSVC)
#  define AUTOBOOST_ASIO_MSVC AUTOBOOST_MSVC
# elif defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
#  define AUTOBOOST_ASIO_MSVC _MSC_VER
# endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_MSVC)
#endif // defined(AUTOBOOST_ASIO_MSVC)

// Clang / libc++ detection.
#if defined(__clang__)
# if (__cplusplus >= 201103)
#  if __has_include(<__config>)
#   include <__config>
#   if defined(_LIBCPP_VERSION)
#    define AUTOBOOST_ASIO_HAS_CLANG_LIBCXX 1
#   endif // defined(_LIBCPP_VERSION)
#  endif // __has_include(<__config>)
# endif // (__cplusplus >= 201103)
#endif // defined(__clang__)

// Support move construction and assignment on compilers known to allow it.
#if !defined(AUTOBOOST_ASIO_HAS_MOVE)
# if !defined(AUTOBOOST_ASIO_DISABLE_MOVE)
#  if defined(__clang__)
#   if __has_feature(__cxx_rvalue_references__)
#    define AUTOBOOST_ASIO_HAS_MOVE 1
#   endif // __has_feature(__cxx_rvalue_references__)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_MOVE 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_MOVE 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_MOVE)
#endif // !defined(AUTOBOOST_ASIO_HAS_MOVE)

// If AUTOBOOST_ASIO_MOVE_CAST isn't defined, and move support is available, define
// AUTOBOOST_ASIO_MOVE_ARG and AUTOBOOST_ASIO_MOVE_CAST to take advantage of rvalue
// references and perfect forwarding.
#if defined(AUTOBOOST_ASIO_HAS_MOVE) && !defined(AUTOBOOST_ASIO_MOVE_CAST)
# define AUTOBOOST_ASIO_MOVE_ARG(type) type&&
# define AUTOBOOST_ASIO_MOVE_CAST(type) static_cast<type&&>
# define AUTOBOOST_ASIO_MOVE_CAST2(type1, type2) static_cast<type1, type2&&>
#endif // defined(AUTOBOOST_ASIO_HAS_MOVE) && !defined(AUTOBOOST_ASIO_MOVE_CAST)

// If AUTOBOOST_ASIO_MOVE_CAST still isn't defined, default to a C++03-compatible
// implementation. Note that older g++ and MSVC versions don't like it when you
// pass a non-member function through a const reference, so for most compilers
// we'll play it safe and stick with the old approach of passing the handler by
// value.
#if !defined(AUTOBOOST_ASIO_MOVE_CAST)
# if defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || (__GNUC__ > 4)
#   define AUTOBOOST_ASIO_MOVE_ARG(type) const type&
#  else // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || (__GNUC__ > 4)
#   define AUTOBOOST_ASIO_MOVE_ARG(type) type
#  endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || (__GNUC__ > 4)
# elif defined(AUTOBOOST_ASIO_MSVC)
#  if (_MSC_VER >= 1400)
#   define AUTOBOOST_ASIO_MOVE_ARG(type) const type&
#  else // (_MSC_VER >= 1400)
#   define AUTOBOOST_ASIO_MOVE_ARG(type) type
#  endif // (_MSC_VER >= 1400)
# else
#  define AUTOBOOST_ASIO_MOVE_ARG(type) type
# endif
# define AUTOBOOST_ASIO_MOVE_CAST(type) static_cast<const type&>
# define AUTOBOOST_ASIO_MOVE_CAST2(type1, type2) static_cast<const type1, type2&>
#endif // !defined(AUTOBOOST_ASIO_MOVE_CAST)

// Support variadic templates on compilers known to allow it.
#if !defined(AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES)
# if !defined(AUTOBOOST_ASIO_DISABLE_VARIADIC_TEMPLATES)
#  if defined(__clang__)
#   if __has_feature(__cxx_variadic_templates__)
#    define AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES 1
#   endif // __has_feature(__cxx_variadic_templates__)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_VARIADIC_TEMPLATES)
#endif // !defined(AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES)

// Support constexpr on compilers known to allow it.
#if !defined(AUTOBOOST_ASIO_HAS_CONSTEXPR)
# if !defined(AUTOBOOST_ASIO_DISABLE_CONSTEXPR)
#  if defined(__clang__)
#   if __has_feature(__cxx_constexpr__)
#    define AUTOBOOST_ASIO_HAS_CONSTEXPR 1
#   endif // __has_feature(__cxx_constexr__)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_CONSTEXPR 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_CONSTEXPR)
#endif // !defined(AUTOBOOST_ASIO_HAS_CONSTEXPR)
#if !defined(AUTOBOOST_ASIO_CONSTEXPR)
# if defined(AUTOBOOST_ASIO_HAS_CONSTEXPR)
#  define AUTOBOOST_ASIO_CONSTEXPR constexpr
# else // defined(AUTOBOOST_ASIO_HAS_CONSTEXPR)
#  define AUTOBOOST_ASIO_CONSTEXPR
# endif // defined(AUTOBOOST_ASIO_HAS_CONSTEXPR)
#endif // !defined(AUTOBOOST_ASIO_CONSTEXPR)

// Standard library support for system errors.
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_SYSTEM_ERROR)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_SYSTEM_ERROR 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<system_error>)
#     define AUTOBOOST_ASIO_HAS_STD_SYSTEM_ERROR 1
#    endif // __has_include(<system_error>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_SYSTEM_ERROR 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_SYSTEM_ERROR 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_SYSTEM_ERROR)

// Compliant C++11 compilers put noexcept specifiers on error_category members.
#if !defined(AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT)
# if (AUTOBOOST_VERSION >= 105300)
#  define AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT AUTOBOOST_NOEXCEPT
# elif defined(__clang__)
#  if __has_feature(__cxx_noexcept__)
#   define AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT noexcept(true)
#  endif // __has_feature(__cxx_noexcept__)
# elif defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT noexcept(true)
#   endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#  endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
# elif defined(AUTOBOOST_ASIO_MSVC)
#  if (_MSC_VER >= 1900)
#   define AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT noexcept(true)
#  endif // (_MSC_VER >= 1900)
# endif // defined(AUTOBOOST_ASIO_MSVC)
# if !defined(AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT)
#  define AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT
# endif // !defined(AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT)
#endif // !defined(AUTOBOOST_ASIO_ERROR_CATEGORY_NOEXCEPT)

// Standard library support for arrays.
#if !defined(AUTOBOOST_ASIO_HAS_STD_ARRAY)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_ARRAY)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_ARRAY 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<array>)
#     define AUTOBOOST_ASIO_HAS_STD_ARRAY 1
#    endif // __has_include(<array>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_ARRAY 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1600)
#    define AUTOBOOST_ASIO_HAS_STD_ARRAY 1
#   endif // (_MSC_VER >= 1600)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_ARRAY)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_ARRAY)

// Standard library support for shared_ptr and weak_ptr.
#if !defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_SHARED_PTR)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_SHARED_PTR 1
#   elif (__cplusplus >= 201103)
#    define AUTOBOOST_ASIO_HAS_STD_SHARED_PTR 1
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_SHARED_PTR 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1600)
#    define AUTOBOOST_ASIO_HAS_STD_SHARED_PTR 1
#   endif // (_MSC_VER >= 1600)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_SHARED_PTR)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_SHARED_PTR)

// Standard library support for atomic operations.
#if !defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_ATOMIC)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_ATOMIC 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<atomic>)
#     define AUTOBOOST_ASIO_HAS_STD_ATOMIC 1
#    endif // __has_include(<atomic>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_ATOMIC 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_ATOMIC 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_ATOMIC)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_ATOMIC)

// Standard library support for chrono. Some standard libraries (such as the
// libstdc++ shipped with gcc 4.6) provide monotonic_clock as per early C++0x
// drafts, rather than the eventually standardised name of steady_clock.
#if !defined(AUTOBOOST_ASIO_HAS_STD_CHRONO)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_CHRONO)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_CHRONO 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<chrono>)
#     define AUTOBOOST_ASIO_HAS_STD_CHRONO 1
#    endif // __has_include(<chrono>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_CHRONO 1
#     if ((__GNUC__ == 4) && (__GNUC_MINOR__ == 6))
#      define AUTOBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK 1
#     endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ == 6))
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_CHRONO 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_CHRONO)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_CHRONO)

// Boost support for chrono.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_CHRONO)
#  if (AUTOBOOST_VERSION >= 104700)
#   define AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO 1
#  endif // (AUTOBOOST_VERSION >= 104700)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_CHRONO)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CHRONO)

// Boost support for the DateTime library.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_DATE_TIME)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_DATE_TIME)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_DATE_TIME 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_DATE_TIME)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_DATE_TIME)

// Standard library support for addressof.
#if !defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_ADDRESSOF)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_ADDRESSOF 1
#   elif (__cplusplus >= 201103)
#    define AUTOBOOST_ASIO_HAS_STD_ADDRESSOF 1
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_ADDRESSOF 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_ADDRESSOF 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_ADDRESSOF)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_ADDRESSOF)

// Standard library support for the function class.
#if !defined(AUTOBOOST_ASIO_HAS_STD_FUNCTION)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_FUNCTION)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_FUNCTION 1
#   elif (__cplusplus >= 201103)
#    define AUTOBOOST_ASIO_HAS_STD_FUNCTION 1
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_FUNCTION 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_FUNCTION 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_FUNCTION)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_FUNCTION)

// Standard library support for type traits.
#if !defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_TYPE_TRAITS)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<type_traits>)
#     define AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS 1
#    endif // __has_include(<type_traits>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_TYPE_TRAITS)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)

// Standard library support for the C++11 allocator additions.
#if !defined(AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS)
# if !defined(AUTOBOOST_ASIO_DISABLE_CXX11_ALLOCATORS)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS 1
#   elif (__cplusplus >= 201103)
#    define AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS 1
#   endif // (__cplusplus >= 201103)
#  elif defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_CXX11_ALLOCATORS)
#endif // !defined(AUTOBOOST_ASIO_HAS_CXX11_ALLOCATORS)

// Standard library support for the cstdint header.
#if !defined(AUTOBOOST_ASIO_HAS_CSTDINT)
# if !defined(AUTOBOOST_ASIO_DISABLE_CSTDINT)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_CSTDINT 1
#   elif (__cplusplus >= 201103)
#    define AUTOBOOST_ASIO_HAS_CSTDINT 1
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_CSTDINT 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_CSTDINT 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_CSTDINT)
#endif // !defined(AUTOBOOST_ASIO_HAS_CSTDINT)

// Standard library support for the thread class.
#if !defined(AUTOBOOST_ASIO_HAS_STD_THREAD)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_THREAD)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_THREAD 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<thread>)
#     define AUTOBOOST_ASIO_HAS_STD_THREAD 1
#    endif // __has_include(<thread>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_THREAD 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_THREAD 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_THREAD)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_THREAD)

// Standard library support for the mutex and condition variable classes.
#if !defined(AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR)
# if !defined(AUTOBOOST_ASIO_DISABLE_STD_MUTEX_AND_CONDVAR)
#  if defined(__clang__)
#   if defined(AUTOBOOST_ASIO_HAS_CLANG_LIBCXX)
#    define AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR 1
#   elif (__cplusplus >= 201103)
#    if __has_include(<mutex>)
#     define AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR 1
#    endif // __has_include(<mutex>)
#   endif // (__cplusplus >= 201103)
#  endif // defined(__clang__)
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR 1
#    endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#   endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#  endif // defined(__GNUC__)
#  if defined(AUTOBOOST_ASIO_MSVC)
#   if (_MSC_VER >= 1700)
#    define AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR 1
#   endif // (_MSC_VER >= 1700)
#  endif // defined(AUTOBOOST_ASIO_MSVC)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_STD_MUTEX_AND_CONDVAR)
#endif // !defined(AUTOBOOST_ASIO_HAS_STD_MUTEX_AND_CONDVAR)

// Windows App target. Windows but with a limited API.
#if !defined(AUTOBOOST_ASIO_WINDOWS_APP)
# if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0603)
#  include <winapifamily.h>
#  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) \
   && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   define AUTOBOOST_ASIO_WINDOWS_APP 1
#  endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
         // && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
# endif // defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0603)
#endif // !defined(AUTOBOOST_ASIO_WINDOWS_APP)

// Legacy WinRT target. Windows App is preferred.
#if !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
# if !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#  if defined(__cplusplus_winrt)
#   include <winapifamily.h>
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) \
    && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#    define AUTOBOOST_ASIO_WINDOWS_RUNTIME 1
#   endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
          // && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#  endif // defined(__cplusplus_winrt)
# endif // !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#endif // !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)

// Windows target. Excludes WinRT but includes Windows App targets.
#if !defined(AUTOBOOST_ASIO_WINDOWS)
# if !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
#  if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_WINDOWS)
#   define AUTOBOOST_ASIO_WINDOWS 1
#  elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define AUTOBOOST_ASIO_WINDOWS 1
#  elif defined(AUTOBOOST_ASIO_WINDOWS_APP)
#   define AUTOBOOST_ASIO_WINDOWS 1
#  endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_WINDOWS)
# endif // !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
#endif // !defined(AUTOBOOST_ASIO_WINDOWS)

// Windows: target OS version.
#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# if !defined(_WIN32_WINNT) && !defined(_WIN32_WINDOWS)
#  if defined(_MSC_VER) || defined(__BORLANDC__)
#   pragma message( \
  "Please define _WIN32_WINNT or _WIN32_WINDOWS appropriately. For example:\n"\
  "- add -D_WIN32_WINNT=0x0501 to the compiler command line; or\n"\
  "- add _WIN32_WINNT=0x0501 to your project's Preprocessor Definitions.\n"\
  "Assuming _WIN32_WINNT=0x0501 (i.e. Windows XP target).")
#  else // defined(_MSC_VER) || defined(__BORLANDC__)
#   warning Please define _WIN32_WINNT or _WIN32_WINDOWS appropriately.
#   warning For example, add -D_WIN32_WINNT=0x0501 to the compiler command line.
#   warning Assuming _WIN32_WINNT=0x0501 (i.e. Windows XP target).
#  endif // defined(_MSC_VER) || defined(__BORLANDC__)
#  define _WIN32_WINNT 0x0501
# endif // !defined(_WIN32_WINNT) && !defined(_WIN32_WINDOWS)
# if defined(_MSC_VER)
#  if defined(_WIN32) && !defined(WIN32)
#   if !defined(_WINSOCK2API_)
#    define WIN32 // Needed for correct types in winsock2.h
#   else // !defined(_WINSOCK2API_)
#    error Please define the macro WIN32 in your compiler options
#   endif // !defined(_WINSOCK2API_)
#  endif // defined(_WIN32) && !defined(WIN32)
# endif // defined(_MSC_VER)
# if defined(__BORLANDC__)
#  if defined(__WIN32__) && !defined(WIN32)
#   if !defined(_WINSOCK2API_)
#    define WIN32 // Needed for correct types in winsock2.h
#   else // !defined(_WINSOCK2API_)
#    error Please define the macro WIN32 in your compiler options
#   endif // !defined(_WINSOCK2API_)
#  endif // defined(__WIN32__) && !defined(WIN32)
# endif // defined(__BORLANDC__)
# if defined(__CYGWIN__)
#  if !defined(__USE_W32_SOCKETS)
#   error You must add -D__USE_W32_SOCKETS to your compiler options.
#  endif // !defined(__USE_W32_SOCKETS)
# endif // defined(__CYGWIN__)
#endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)

// Windows: minimise header inclusion.
#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# if !defined(AUTOBOOST_ASIO_NO_WIN32_LEAN_AND_MEAN)
#  if !defined(WIN32_LEAN_AND_MEAN)
#   define WIN32_LEAN_AND_MEAN
#  endif // !defined(WIN32_LEAN_AND_MEAN)
# endif // !defined(AUTOBOOST_ASIO_NO_WIN32_LEAN_AND_MEAN)
#endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)

// Windows: suppress definition of "min" and "max" macros.
#if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# if !defined(AUTOBOOST_ASIO_NO_NOMINMAX)
#  if !defined(NOMINMAX)
#   define NOMINMAX 1
#  endif // !defined(NOMINMAX)
# endif // !defined(AUTOBOOST_ASIO_NO_NOMINMAX)
#endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)

// Windows: IO Completion Ports.
#if !defined(AUTOBOOST_ASIO_HAS_IOCP)
# if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
#  if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0400)
#   if !defined(UNDER_CE) && !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#    if !defined(AUTOBOOST_ASIO_DISABLE_IOCP)
#     define AUTOBOOST_ASIO_HAS_IOCP 1
#    endif // !defined(AUTOBOOST_ASIO_DISABLE_IOCP)
#   endif // !defined(UNDER_CE) && !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#  endif // defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0400)
# endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
#endif // !defined(AUTOBOOST_ASIO_HAS_IOCP)

// On POSIX (and POSIX-like) platforms we need to include unistd.h in order to
// get access to the various platform feature macros, e.g. to be able to test
// for threads support.
#if !defined(AUTOBOOST_ASIO_HAS_UNISTD_H)
# if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG)
#  if defined(unix) \
   || defined(__unix) \
   || defined(_XOPEN_SOURCE) \
   || defined(_POSIX_SOURCE) \
   || (defined(__MACH__) && defined(__APPLE__)) \
   || defined(__FreeBSD__) \
   || defined(__NetBSD__) \
   || defined(__OpenBSD__) \
   || defined(__linux__)
#   define AUTOBOOST_ASIO_HAS_UNISTD_H 1
#  endif
# endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG)
#endif // !defined(AUTOBOOST_ASIO_HAS_UNISTD_H)
#if defined(AUTOBOOST_ASIO_HAS_UNISTD_H)
# include <unistd.h>
#endif // defined(AUTOBOOST_ASIO_HAS_UNISTD_H)

// Linux: epoll, eventfd and timerfd.
#if defined(__linux__)
# include <linux/version.h>
# if !defined(AUTOBOOST_ASIO_HAS_EPOLL)
#  if !defined(AUTOBOOST_ASIO_DISABLE_EPOLL)
#   if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,45)
#    define AUTOBOOST_ASIO_HAS_EPOLL 1
#   endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,45)
#  endif // !defined(AUTOBOOST_ASIO_DISABLE_EPOLL)
# endif // !defined(AUTOBOOST_ASIO_HAS_EPOLL)
# if !defined(AUTOBOOST_ASIO_HAS_EVENTFD)
#  if !defined(AUTOBOOST_ASIO_DISABLE_EVENTFD)
#   if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#    define AUTOBOOST_ASIO_HAS_EVENTFD 1
#   endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#  endif // !defined(AUTOBOOST_ASIO_DISABLE_EVENTFD)
# endif // !defined(AUTOBOOST_ASIO_HAS_EVENTFD)
# if !defined(AUTOBOOST_ASIO_HAS_TIMERFD)
#  if defined(AUTOBOOST_ASIO_HAS_EPOLL)
#   if (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 8)
#    define AUTOBOOST_ASIO_HAS_TIMERFD 1
#   endif // (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 8)
#  endif // defined(AUTOBOOST_ASIO_HAS_EPOLL)
# endif // !defined(AUTOBOOST_ASIO_HAS_TIMERFD)
#endif // defined(__linux__)

// Mac OS X, FreeBSD, NetBSD, OpenBSD: kqueue.
#if (defined(__MACH__) && defined(__APPLE__)) \
  || defined(__FreeBSD__) \
  || defined(__NetBSD__) \
  || defined(__OpenBSD__)
# if !defined(AUTOBOOST_ASIO_HAS_KQUEUE)
#  if !defined(AUTOBOOST_ASIO_DISABLE_KQUEUE)
#   define AUTOBOOST_ASIO_HAS_KQUEUE 1
#  endif // !defined(AUTOBOOST_ASIO_DISABLE_KQUEUE)
# endif // !defined(AUTOBOOST_ASIO_HAS_KQUEUE)
#endif // (defined(__MACH__) && defined(__APPLE__))
       //   || defined(__FreeBSD__)
       //   || defined(__NetBSD__)
       //   || defined(__OpenBSD__)

// Solaris: /dev/poll.
#if defined(__sun)
# if !defined(AUTOBOOST_ASIO_HAS_DEV_POLL)
#  if !defined(AUTOBOOST_ASIO_DISABLE_DEV_POLL)
#   define AUTOBOOST_ASIO_HAS_DEV_POLL 1
#  endif // !defined(AUTOBOOST_ASIO_DISABLE_DEV_POLL)
# endif // !defined(AUTOBOOST_ASIO_HAS_DEV_POLL)
#endif // defined(__sun)

// Serial ports.
#if !defined(AUTOBOOST_ASIO_HAS_SERIAL_PORT)
# if defined(AUTOBOOST_ASIO_HAS_IOCP) \
  || !defined(AUTOBOOST_ASIO_WINDOWS) \
  && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)
#  if !defined(__SYMBIAN32__)
#   if !defined(AUTOBOOST_ASIO_DISABLE_SERIAL_PORT)
#    define AUTOBOOST_ASIO_HAS_SERIAL_PORT 1
#   endif // !defined(AUTOBOOST_ASIO_DISABLE_SERIAL_PORT)
#  endif // !defined(__SYMBIAN32__)
# endif // defined(AUTOBOOST_ASIO_HAS_IOCP)
        //   || !defined(AUTOBOOST_ASIO_WINDOWS)
        //   && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
        //   && !defined(__CYGWIN__)
#endif // !defined(AUTOBOOST_ASIO_HAS_SERIAL_PORT)

// Windows: stream handles.
#if !defined(AUTOBOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
# if !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_STREAM_HANDLE)
#  if defined(AUTOBOOST_ASIO_HAS_IOCP)
#   define AUTOBOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE 1
#  endif // defined(AUTOBOOST_ASIO_HAS_IOCP)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_STREAM_HANDLE)
#endif // !defined(AUTOBOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)

// Windows: random access handles.
#if !defined(AUTOBOOST_ASIO_HAS_WINDOWS_RANDOM_ACCESS_HANDLE)
# if !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_RANDOM_ACCESS_HANDLE)
#  if defined(AUTOBOOST_ASIO_HAS_IOCP)
#   define AUTOBOOST_ASIO_HAS_WINDOWS_RANDOM_ACCESS_HANDLE 1
#  endif // defined(AUTOBOOST_ASIO_HAS_IOCP)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_RANDOM_ACCESS_HANDLE)
#endif // !defined(AUTOBOOST_ASIO_HAS_WINDOWS_RANDOM_ACCESS_HANDLE)

// Windows: object handles.
#if !defined(AUTOBOOST_ASIO_HAS_WINDOWS_OBJECT_HANDLE)
# if !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_OBJECT_HANDLE)
#  if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
#   if !defined(UNDER_CE) && !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#    define AUTOBOOST_ASIO_HAS_WINDOWS_OBJECT_HANDLE 1
#   endif // !defined(UNDER_CE) && !defined(AUTOBOOST_ASIO_WINDOWS_APP)
#  endif // defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_OBJECT_HANDLE)
#endif // !defined(AUTOBOOST_ASIO_HAS_WINDOWS_OBJECT_HANDLE)

// Windows: OVERLAPPED wrapper.
#if !defined(AUTOBOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR)
# if !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_OVERLAPPED_PTR)
#  if defined(AUTOBOOST_ASIO_HAS_IOCP)
#   define AUTOBOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR 1
#  endif // defined(AUTOBOOST_ASIO_HAS_IOCP)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_WINDOWS_OVERLAPPED_PTR)
#endif // !defined(AUTOBOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR)

// POSIX: stream-oriented file descriptors.
#if !defined(AUTOBOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
# if !defined(AUTOBOOST_ASIO_DISABLE_POSIX_STREAM_DESCRIPTOR)
#  if !defined(AUTOBOOST_ASIO_WINDOWS) \
  && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)
#   define AUTOBOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR 1
#  endif // !defined(AUTOBOOST_ASIO_WINDOWS)
         //   && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
         //   && !defined(__CYGWIN__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_POSIX_STREAM_DESCRIPTOR)
#endif // !defined(AUTOBOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)

// UNIX domain sockets.
#if !defined(AUTOBOOST_ASIO_HAS_LOCAL_SOCKETS)
# if !defined(AUTOBOOST_ASIO_DISABLE_LOCAL_SOCKETS)
#  if !defined(AUTOBOOST_ASIO_WINDOWS) \
  && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)
#   define AUTOBOOST_ASIO_HAS_LOCAL_SOCKETS 1
#  endif // !defined(AUTOBOOST_ASIO_WINDOWS)
         //   && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
         //   && !defined(__CYGWIN__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_LOCAL_SOCKETS)
#endif // !defined(AUTOBOOST_ASIO_HAS_LOCAL_SOCKETS)

// Can use sigaction() instead of signal().
#if !defined(AUTOBOOST_ASIO_HAS_SIGACTION)
# if !defined(AUTOBOOST_ASIO_DISABLE_SIGACTION)
#  if !defined(AUTOBOOST_ASIO_WINDOWS) \
  && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME) \
  && !defined(__CYGWIN__)
#   define AUTOBOOST_ASIO_HAS_SIGACTION 1
#  endif // !defined(AUTOBOOST_ASIO_WINDOWS)
         //   && !defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
         //   && !defined(__CYGWIN__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_SIGACTION)
#endif // !defined(AUTOBOOST_ASIO_HAS_SIGACTION)

// Can use signal().
#if !defined(AUTOBOOST_ASIO_HAS_SIGNAL)
# if !defined(AUTOBOOST_ASIO_DISABLE_SIGNAL)
#  if !defined(UNDER_CE)
#   define AUTOBOOST_ASIO_HAS_SIGNAL 1
#  endif // !defined(UNDER_CE)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_SIGNAL)
#endif // !defined(AUTOBOOST_ASIO_HAS_SIGNAL)

// Can use getaddrinfo() and getnameinfo().
#if !defined(AUTOBOOST_ASIO_HAS_GETADDRINFO)
# if !defined(AUTOBOOST_ASIO_DISABLE_GETADDRINFO)
#  if defined(AUTOBOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
#   if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0501)
#    define AUTOBOOST_ASIO_HAS_GETADDRINFO 1
#   elif defined(UNDER_CE)
#    define AUTOBOOST_ASIO_HAS_GETADDRINFO 1
#   endif // defined(UNDER_CE)
#  elif defined(__MACH__) && defined(__APPLE__)
#   if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#    if (__MAC_OS_X_VERSION_MIN_REQUIRED >= 1050)
#     define AUTOBOOST_ASIO_HAS_GETADDRINFO 1
#    endif // (__MAC_OS_X_VERSION_MIN_REQUIRED >= 1050)
#   else // defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#    define AUTOBOOST_ASIO_HAS_GETADDRINFO 1
#   endif // defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#  else // defined(__MACH__) && defined(__APPLE__)
#   define AUTOBOOST_ASIO_HAS_GETADDRINFO 1
#  endif // defined(__MACH__) && defined(__APPLE__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_GETADDRINFO)
#endif // !defined(AUTOBOOST_ASIO_HAS_GETADDRINFO)

// Whether standard iostreams are disabled.
#if !defined(AUTOBOOST_ASIO_NO_IOSTREAM)
# if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_NO_IOSTREAM)
#  define AUTOBOOST_ASIO_NO_IOSTREAM 1
# endif // !defined(AUTOBOOST_NO_IOSTREAM)
#endif // !defined(AUTOBOOST_ASIO_NO_IOSTREAM)

// Whether exception handling is disabled.
#if !defined(AUTOBOOST_ASIO_NO_EXCEPTIONS)
# if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_NO_EXCEPTIONS)
#  define AUTOBOOST_ASIO_NO_EXCEPTIONS 1
# endif // !defined(AUTOBOOST_NO_EXCEPTIONS)
#endif // !defined(AUTOBOOST_ASIO_NO_EXCEPTIONS)

// Whether the typeid operator is supported.
#if !defined(AUTOBOOST_ASIO_NO_TYPEID)
# if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_NO_TYPEID)
#  define AUTOBOOST_ASIO_NO_TYPEID 1
# endif // !defined(AUTOBOOST_NO_TYPEID)
#endif // !defined(AUTOBOOST_ASIO_NO_TYPEID)

// Threads.
#if !defined(AUTOBOOST_ASIO_HAS_THREADS)
# if !defined(AUTOBOOST_ASIO_DISABLE_THREADS)
#  if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_HAS_THREADS)
#   define AUTOBOOST_ASIO_HAS_THREADS 1
#  elif defined(_MSC_VER) && defined(_MT)
#   define AUTOBOOST_ASIO_HAS_THREADS 1
#  elif defined(__BORLANDC__) && defined(__MT__)
#   define AUTOBOOST_ASIO_HAS_THREADS 1
#  elif defined(_POSIX_THREADS)
#   define AUTOBOOST_ASIO_HAS_THREADS 1
#  endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_HAS_THREADS)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_THREADS)
#endif // !defined(AUTOBOOST_ASIO_HAS_THREADS)

// POSIX threads.
#if !defined(AUTOBOOST_ASIO_HAS_PTHREADS)
# if defined(AUTOBOOST_ASIO_HAS_THREADS)
#  if defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_HAS_PTHREADS)
#   define AUTOBOOST_ASIO_HAS_PTHREADS 1
#  elif defined(_POSIX_THREADS)
#   define AUTOBOOST_ASIO_HAS_PTHREADS 1
#  endif // defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_CONFIG) && defined(AUTOBOOST_HAS_PTHREADS)
# endif // defined(AUTOBOOST_ASIO_HAS_THREADS)
#endif // !defined(AUTOBOOST_ASIO_HAS_PTHREADS)

// Helper to prevent macro expansion.
#define AUTOBOOST_ASIO_PREVENT_MACRO_SUBSTITUTION

// Helper to define in-class constants.
#if !defined(AUTOBOOST_ASIO_STATIC_CONSTANT)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_STATIC_CONSTANT)
#  define AUTOBOOST_ASIO_STATIC_CONSTANT(type, assignment) \
    AUTOBOOST_STATIC_CONSTANT(type, assignment)
# else // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_STATIC_CONSTANT)
#  define AUTOBOOST_ASIO_STATIC_CONSTANT(type, assignment) \
    static const type assignment
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_STATIC_CONSTANT)
#endif // !defined(AUTOBOOST_ASIO_STATIC_CONSTANT)

// Boost array library.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ARRAY)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ARRAY)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_ARRAY 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ARRAY)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ARRAY)

// Boost assert macro.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ASSERT)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_ASSERT)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_ASSERT)

// Boost limits header.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_LIMITS)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_LIMITS)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_LIMITS)

// Boost throw_exception function.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_THROW_EXCEPTION)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_THROW_EXCEPTION)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_THROW_EXCEPTION)

// Boost regex library.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_REGEX)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_REGEX)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_REGEX 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_REGEX)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_REGEX)

// Boost bind function.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_BIND)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_BIND)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_BIND 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_BIND)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_BIND)

// Boost's AUTOBOOST_WORKAROUND macro.
#if !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_WORKAROUND)
# if !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_WORKAROUND)
#  define AUTOBOOST_ASIO_HAS_AUTOBOOST_WORKAROUND 1
# endif // !defined(AUTOBOOST_ASIO_DISABLE_AUTOBOOST_WORKAROUND)
#endif // !defined(AUTOBOOST_ASIO_HAS_AUTOBOOST_WORKAROUND)

// Microsoft Visual C++'s secure C runtime library.
#if !defined(AUTOBOOST_ASIO_HAS_SECURE_RTL)
# if !defined(AUTOBOOST_ASIO_DISABLE_SECURE_RTL)
#  if defined(AUTOBOOST_ASIO_MSVC) \
    && (AUTOBOOST_ASIO_MSVC >= 1400) \
    && !defined(UNDER_CE)
#   define AUTOBOOST_ASIO_HAS_SECURE_RTL 1
#  endif // defined(AUTOBOOST_ASIO_MSVC)
         // && (AUTOBOOST_ASIO_MSVC >= 1400)
         // && !defined(UNDER_CE)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_SECURE_RTL)
#endif // !defined(AUTOBOOST_ASIO_HAS_SECURE_RTL)

// Handler hooking. Disabled for ancient Borland C++ and gcc compilers.
#if !defined(AUTOBOOST_ASIO_HAS_HANDLER_HOOKS)
# if !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_HOOKS)
#  if defined(__GNUC__)
#   if (__GNUC__ >= 3)
#    define AUTOBOOST_ASIO_HAS_HANDLER_HOOKS 1
#   endif // (__GNUC__ >= 3)
#  elif !defined(__BORLANDC__)
#   define AUTOBOOST_ASIO_HAS_HANDLER_HOOKS 1
#  endif // !defined(__BORLANDC__)
# endif // !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_HOOKS)
#endif // !defined(AUTOBOOST_ASIO_HAS_HANDLER_HOOKS)

// Support for the __thread keyword extension.
#if !defined(AUTOBOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION)
# if defined(__linux__)
#  if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#   if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 3)
#    if !defined(__INTEL_COMPILER) && !defined(__ICL)
#     define AUTOBOOST_ASIO_HAS_THREAD_KEYWORD_EXTENSION 1
#     define AUTOBOOST_ASIO_THREAD_KEYWORD __thread
#    elif defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1100)
#     define AUTOBOOST_ASIO_HAS_THREAD_KEYWORD_EXTENSION 1
#    endif // defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1100)
#   endif // ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 3)
#  endif // defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
# endif // defined(__linux__)
# if defined(AUTOBOOST_ASIO_MSVC) && defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
#  if (_MSC_VER >= 1700)
#   define AUTOBOOST_ASIO_HAS_THREAD_KEYWORD_EXTENSION 1
#   define AUTOBOOST_ASIO_THREAD_KEYWORD __declspec(thread)
#  endif // (_MSC_VER >= 1700)
# endif // defined(AUTOBOOST_ASIO_MSVC) && defined(AUTOBOOST_ASIO_WINDOWS_RUNTIME)
#endif // !defined(AUTOBOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION)
#if !defined(AUTOBOOST_ASIO_THREAD_KEYWORD)
# define AUTOBOOST_ASIO_THREAD_KEYWORD __thread
#endif // !defined(AUTOBOOST_ASIO_THREAD_KEYWORD)

// Support for POSIX ssize_t typedef.
#if !defined(AUTOBOOST_ASIO_DISABLE_SSIZE_T)
# if defined(__linux__) \
   || (defined(__MACH__) && defined(__APPLE__))
#  define AUTOBOOST_ASIO_HAS_SSIZE_T 1
# endif // defined(__linux__)
        //   || (defined(__MACH__) && defined(__APPLE__))
#endif // !defined(AUTOBOOST_ASIO_DISABLE_SSIZE_T)

// Newer gcc, clang need special treatment to suppress unused typedef warnings.
#if defined(__clang__)
# if defined(__apple_build_version__)
#  if (__clang_major__ >= 7)
#   define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
#  endif // (__clang_major__ >= 7)
# elif ((__clang_major__ == 3) && (__clang_minor__ >= 6)) \
    || (__clang_major__ > 3)
#  define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
# endif // ((__clang_major__ == 3) && (__clang_minor__ >= 6))
        //   || (__clang_major__ > 3)
#elif defined(__GNUC__)
# if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4)
#  define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
# endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4)
#endif // defined(__GNUC__)
#if !defined(AUTOBOOST_ASIO_UNUSED_TYPEDEF)
# define AUTOBOOST_ASIO_UNUSED_TYPEDEF
#endif // !defined(AUTOBOOST_ASIO_UNUSED_TYPEDEF)

// Some versions of gcc generate spurious warnings about unused variables.
#if defined(__GNUC__)
# if (__GNUC__ >= 4)
#  define AUTOBOOST_ASIO_UNUSED_VARIABLE __attribute__((__unused__))
# endif // (__GNUC__ >= 4)
#endif // defined(__GNUC__)
#if !defined(AUTOBOOST_ASIO_UNUSED_VARIABLE)
# define AUTOBOOST_ASIO_UNUSED_VARIABLE
#endif // !defined(AUTOBOOST_ASIO_UNUSED_VARIABLE)

#endif // AUTOBOOST_ASIO_DETAIL_CONFIG_HPP
