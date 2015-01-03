//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP
#define AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP

//! \file
//! This header file forward declares autoboost::container::scoped_allocator_adaptor
//! and defines the following types:

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/container/detail/preprocessor.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#endif

namespace autoboost { namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)

   template <typename OuterAlloc, typename ...InnerAllocs>
   class scoped_allocator_adaptor;

   #else // #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)

   template <typename ...InnerAllocs>
   class scoped_allocator_adaptor;

   template <typename OuterAlloc, typename ...InnerAllocs>
   class scoped_allocator_adaptor<OuterAlloc, InnerAllocs...>;

   #endif   // #if !defined(AUTOBOOST_CONTAINER_UNIMPLEMENTED_PACK_EXPANSION_TO_FIXED_LIST)


#else    // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

template <typename OuterAlloc
AUTOBOOST_PP_ENUM_TRAILING( AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS
                      , AUTOBOOST_CONTAINER_PP_TEMPLATE_PARAM_WITH_DEFAULT, container_detail::nat)
>
class scoped_allocator_adaptor;

#endif

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! The allocator_arg_t struct is an empty structure type used as a unique type to
//! disambiguate constructor and function overloading. Specifically, several types
//! have constructors with allocator_arg_t as the first argument, immediately followed
//! by an argument of a type that satisfies the Allocator requirements
struct allocator_arg_t{};

//! A instance of type allocator_arg_t
//!
static const allocator_arg_t allocator_arg = allocator_arg_t();

template <class T>
struct constructible_with_allocator_suffix;

template <class T>
struct constructible_with_allocator_prefix;

template <typename T, typename Alloc>
struct uses_allocator;

}} // namespace autoboost { namespace container {

#include <autoboost/container/detail/config_end.hpp>

#endif //  AUTOBOOST_CONTAINER_ALLOCATOR_SCOPED_ALLOCATOR_FWD_HPP
