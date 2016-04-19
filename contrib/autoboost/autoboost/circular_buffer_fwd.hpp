// Forward declaration of the circular buffer and its adaptor.

// Copyright (c) 2003-2008 Jan Gaspar

// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See www.boost.org/libs/circular_buffer for documentation.

#if !defined(AUTOBOOST_CIRCULAR_BUFFER_FWD_HPP)
#define AUTOBOOST_CIRCULAR_BUFFER_FWD_HPP

#if defined(_MSC_VER)
    #pragma once
#endif

#include <autoboost/config.hpp>
#if !defined(AUTOBOOST_NO_STD_ALLOCATOR)
    #include <memory>
#else
    #include <vector>
#endif

namespace autoboost {

#if !defined(AUTOBOOST_NO_STD_ALLOCATOR)
    #define AUTOBOOST_CB_DEFAULT_ALLOCATOR(T) std::allocator<T>
#else
    #define AUTOBOOST_CB_DEFAULT_ALLOCATOR(T) AUTOBOOST_DEDUCED_TYPENAME std::vector<T>::allocator_type
#endif

template <class T, class Alloc = AUTOBOOST_CB_DEFAULT_ALLOCATOR(T)>
class circular_buffer;

template <class T, class Alloc = AUTOBOOST_CB_DEFAULT_ALLOCATOR(T)>
class circular_buffer_space_optimized;

#undef AUTOBOOST_CB_DEFAULT_ALLOCATOR

} // namespace autoboost

#endif // #if !defined(AUTOBOOST_CIRCULAR_BUFFER_FWD_HPP)
