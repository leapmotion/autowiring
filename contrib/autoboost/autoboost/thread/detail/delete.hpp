// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_DETAIL_DELETE_HPP
#define AUTOBOOST_THREAD_DETAIL_DELETE_HPP

#include <autoboost/config.hpp>

/**
 * AUTOBOOST_THREAD_DELETE_COPY_CTOR deletes the copy constructor when the compiler supports it or
 * makes it private.
 *
 * AUTOBOOST_THREAD_DELETE_COPY_ASSIGN deletes the copy assignment when the compiler supports it or
 * makes it private.
 */

#if ! defined AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS && ! defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_THREAD_DELETE_COPY_CTOR(CLASS) \
      CLASS(CLASS const&) = delete; \

#define AUTOBOOST_THREAD_DELETE_COPY_ASSIGN(CLASS) \
      CLASS& operator=(CLASS const&) = delete;

#else // AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#if defined(AUTOBOOST_MSVC) && _MSC_VER >= 1600
#define AUTOBOOST_THREAD_DELETE_COPY_CTOR(CLASS) \
    private: \
      CLASS(CLASS const&); \
    public:

#define AUTOBOOST_THREAD_DELETE_COPY_ASSIGN(CLASS) \
    private: \
      CLASS& operator=(CLASS const&); \
    public:
#else
#define AUTOBOOST_THREAD_DELETE_COPY_CTOR(CLASS) \
    private: \
      CLASS(CLASS&); \
    public:

#define AUTOBOOST_THREAD_DELETE_COPY_ASSIGN(CLASS) \
    private: \
      CLASS& operator=(CLASS&); \
    public:
#endif
#endif // AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS

/**
 * AUTOBOOST_THREAD_NO_COPYABLE deletes the copy constructor and assignment when the compiler supports it or
 * makes them private.
 */
#define AUTOBOOST_THREAD_NO_COPYABLE(CLASS) \
    AUTOBOOST_THREAD_DELETE_COPY_CTOR(CLASS) \
    AUTOBOOST_THREAD_DELETE_COPY_ASSIGN(CLASS)

#endif // AUTOBOOST_THREAD_DETAIL_DELETE_HPP
