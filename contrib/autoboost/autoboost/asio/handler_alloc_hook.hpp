//
// handler_alloc_hook.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_HANDLER_ALLOC_HOOK_HPP
#define AUTOBOOST_ASIO_HANDLER_ALLOC_HOOK_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <cstddef>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {

/// Default allocation function for handlers.
/**
 * Asynchronous operations may need to allocate temporary objects. Since
 * asynchronous operations have a handler function object, these temporary
 * objects can be said to be associated with the handler.
 *
 * Implement asio_handler_allocate and asio_handler_deallocate for your own
 * handlers to provide custom allocation for these temporary objects.
 *
 * The default implementation of these allocation hooks uses <tt>::operator
 * new</tt> and <tt>::operator delete</tt>.
 *
 * @note All temporary objects associated with a handler will be deallocated
 * before the upcall to the handler is performed. This allows the same memory to
 * be reused for a subsequent asynchronous operation initiated by the handler.
 *
 * @par Example
 * @code
 * class my_handler;
 *
 * void* asio_handler_allocate(std::size_t size, my_handler* context)
 * {
 *   return ::operator new(size);
 * }
 *
 * void asio_handler_deallocate(void* pointer, std::size_t size,
 *     my_handler* context)
 * {
 *   ::operator delete(pointer);
 * }
 * @endcode
 */
AUTOBOOST_ASIO_DECL void* asio_handler_allocate(
    std::size_t size, ...);

/// Default deallocation function for handlers.
/**
 * Implement asio_handler_allocate and asio_handler_deallocate for your own
 * handlers to provide custom allocation for the associated temporary objects.
 *
 * The default implementation of these allocation hooks uses <tt>::operator
 * new</tt> and <tt>::operator delete</tt>.
 *
 * @sa asio_handler_allocate.
 */
AUTOBOOST_ASIO_DECL void asio_handler_deallocate(
    void* pointer, std::size_t size, ...);

} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/impl/handler_alloc_hook.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_HANDLER_ALLOC_HOOK_HPP
