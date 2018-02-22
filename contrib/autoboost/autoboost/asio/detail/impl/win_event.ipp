//
// detail/win_event.ipp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_IMPL_WIN_EVENT_IPP
#define AUTOBOOST_ASIO_DETAIL_IMPL_WIN_EVENT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_WINDOWS)

#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/detail/win_event.hpp>
#include <autoboost/asio/error.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

win_event::win_event()
  : state_(0)
{
#if defined(AUTOBOOST_ASIO_WINDOWS_APP)
  events_[0] = ::CreateEventExW(0, 0, CREATE_EVENT_MANUAL_RESET, 0);
#else // defined(AUTOBOOST_ASIO_WINDOWS_APP)
  events_[0] = ::CreateEventW(0, true, false, 0);
#endif // defined(AUTOBOOST_ASIO_WINDOWS_APP)
  if (!events_[0])
  {
    DWORD last_error = ::GetLastError();
    autoboost::system::error_code ec(last_error,
        autoboost::asio::error::get_system_category());
    autoboost::asio::detail::throw_error(ec, "event");
  }

#if defined(AUTOBOOST_ASIO_WINDOWS_APP)
  events_[1] = ::CreateEventExW(0, 0, 0, 0);
#else // defined(AUTOBOOST_ASIO_WINDOWS_APP)
  events_[1] = ::CreateEventW(0, false, false, 0);
#endif // defined(AUTOBOOST_ASIO_WINDOWS_APP)
  if (!events_[1])
  {
    DWORD last_error = ::GetLastError();
    ::CloseHandle(events_[0]);
    autoboost::system::error_code ec(last_error,
        autoboost::asio::error::get_system_category());
    autoboost::asio::detail::throw_error(ec, "event");
  }
}

win_event::~win_event()
{
  ::CloseHandle(events_[0]);
  ::CloseHandle(events_[1]);
}

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_WINDOWS)

#endif // AUTOBOOST_ASIO_DETAIL_IMPL_WIN_EVENT_IPP
