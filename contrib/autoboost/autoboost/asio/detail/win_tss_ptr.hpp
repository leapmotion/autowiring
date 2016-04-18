//
// detail/win_tss_ptr.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WIN_TSS_PTR_HPP
#define AUTOBOOST_ASIO_DETAIL_WIN_TSS_PTR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS)

#include <autoboost/asio/detail/noncopyable.hpp>
#include <autoboost/asio/detail/socket_types.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

// Helper function to create thread-specific storage.
AUTOBOOST_ASIO_DECL DWORD win_tss_ptr_create();

template <typename T>
class win_tss_ptr
  : private noncopyable
{
public:
  // Constructor.
  win_tss_ptr()
    : tss_key_(win_tss_ptr_create())
  {
  }

  // Destructor.
  ~win_tss_ptr()
  {
    ::TlsFree(tss_key_);
  }

  // Get the value.
  operator T*() const
  {
    return static_cast<T*>(::TlsGetValue(tss_key_));
  }

  // Set the value.
  void operator=(T* value)
  {
    ::TlsSetValue(tss_key_, value);
  }

private:
  // Thread-specific storage to allow unlocked access to determine whether a
  // thread is a member of the pool.
  DWORD tss_key_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/win_tss_ptr.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_WINDOWS)

#endif // AUTOBOOST_ASIO_DETAIL_WIN_TSS_PTR_HPP
