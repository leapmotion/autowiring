// Circular buffer library header file.

// Copyright (c) 2003-2008 Jan Gaspar

// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See www.boost.org/libs/circular_buffer for documentation.

#if !defined(AUTOBOOST_CIRCULAR_BUFFER_HPP)
#define AUTOBOOST_CIRCULAR_BUFFER_HPP

#if defined(_MSC_VER)
    #pragma once
#endif

#include <autoboost/circular_buffer_fwd.hpp>
#include <autoboost/detail/workaround.hpp>
#include <autoboost/static_assert.hpp>

// AUTOBOOST_CB_ENABLE_DEBUG: Debug support control.
#if defined(NDEBUG) || defined(AUTOBOOST_CB_DISABLE_DEBUG)
    #define AUTOBOOST_CB_ENABLE_DEBUG 0
#else
    #define AUTOBOOST_CB_ENABLE_DEBUG 1
#endif

// AUTOBOOST_CB_ASSERT: Runtime assertion.
#if AUTOBOOST_CB_ENABLE_DEBUG
    #include <autoboost/assert.hpp>
    #define AUTOBOOST_CB_ASSERT(Expr) AUTOBOOST_ASSERT(Expr)
#else
    #define AUTOBOOST_CB_ASSERT(Expr) ((void)0)
#endif

// AUTOBOOST_CB_IS_CONVERTIBLE: Check if Iterator::value_type is convertible to Type.
#if AUTOBOOST_WORKAROUND(__BORLANDC__, <= 0x0550) || AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x2407)
    #define AUTOBOOST_CB_IS_CONVERTIBLE(Iterator, Type) ((void)0)
#else
    #include <autoboost/detail/iterator.hpp>
    #include <autoboost/type_traits/is_convertible.hpp>
    #define AUTOBOOST_CB_IS_CONVERTIBLE(Iterator, Type) \
        AUTOBOOST_STATIC_ASSERT((is_convertible<typename detail::iterator_traits<Iterator>::value_type, Type>::value))
#endif

// AUTOBOOST_CB_ASSERT_TEMPLATED_ITERATOR_CONSTRUCTORS:
// Check if the STL provides templated iterator constructors for its containers.
#if defined(AUTOBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS)
    #define AUTOBOOST_CB_ASSERT_TEMPLATED_ITERATOR_CONSTRUCTORS AUTOBOOST_STATIC_ASSERT(false);
#else
    #define AUTOBOOST_CB_ASSERT_TEMPLATED_ITERATOR_CONSTRUCTORS ((void)0);
#endif

#include <autoboost/circular_buffer/debug.hpp>
#include <autoboost/circular_buffer/details.hpp>
#include <autoboost/circular_buffer/base.hpp>
#include <autoboost/circular_buffer/space_optimized.hpp>

#undef AUTOBOOST_CB_ASSERT_TEMPLATED_ITERATOR_CONSTRUCTORS
#undef AUTOBOOST_CB_IS_CONVERTIBLE
#undef AUTOBOOST_CB_ASSERT
#undef AUTOBOOST_CB_ENABLE_DEBUG

#endif // #if !defined(AUTOBOOST_CIRCULAR_BUFFER_HPP)
