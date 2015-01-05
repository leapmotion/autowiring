/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2012 Hartmut Kaiser
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/config.hpp
 *
 * This header defines macros for linking with compiled library of Boost.Atomic
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_LINK_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_LINK_HPP_INCLUDED_

#include <autoboost/atomic/detail/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

///////////////////////////////////////////////////////////////////////////////
//  Set up dll import/export options
#if (defined(AUTOBOOST_ATOMIC_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)) && \
    !defined(AUTOBOOST_ATOMIC_STATIC_LINK)

#if defined(AUTOBOOST_ATOMIC_SOURCE)
#define AUTOBOOST_ATOMIC_DECL AUTOBOOST_SYMBOL_EXPORT
#define AUTOBOOST_ATOMIC_BUILD_DLL
#else
#define AUTOBOOST_ATOMIC_DECL AUTOBOOST_SYMBOL_IMPORT
#endif

#endif // building a shared library

#ifndef AUTOBOOST_ATOMIC_DECL
#define AUTOBOOST_ATOMIC_DECL
#endif

///////////////////////////////////////////////////////////////////////////////
//  Auto library naming
#if !defined(AUTOBOOST_ATOMIC_SOURCE) && !defined(AUTOBOOST_ALL_NO_LIB) && \
    !defined(AUTOBOOST_ATOMIC_NO_LIB)

#define AUTOBOOST_LIB_NAME autoboost_atomic

// tell the auto-link code to select a dll when required:
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_ATOMIC_DYN_LINK)
#define AUTOBOOST_DYN_LINK
#endif

#include <autoboost/config/auto_link.hpp>

#endif  // auto-linking disabled

#endif
