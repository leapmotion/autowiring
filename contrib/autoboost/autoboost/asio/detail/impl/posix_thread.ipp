//
// detail/impl/posix_thread.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_THREAD_IPP
#define AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_THREAD_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_PTHREADS)

#include <autoboost/asio/detail/posix_thread.hpp>
#include <autoboost/asio/detail/throw_error.hpp>
#include <autoboost/asio/error.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

posix_thread::~posix_thread()
{
  if (!joined_)
    ::pthread_detach(thread_);
}

void posix_thread::join()
{
  if (!joined_)
  {
    ::pthread_join(thread_, 0);
    joined_ = true;
  }
}

void posix_thread::start_thread(func_base* arg)
{
  int error = ::pthread_create(&thread_, 0,
        autoboost_asio_detail_posix_thread_function, arg);
  if (error != 0)
  {
    delete arg;
    autoboost::system::error_code ec(error,
        autoboost::asio::error::get_system_category());
    autoboost::asio::detail::throw_error(ec, "thread");
  }
}

void* autoboost_asio_detail_posix_thread_function(void* arg)
{
  posix_thread::auto_func_base_ptr func = {
      static_cast<posix_thread::func_base*>(arg) };
  func.ptr->run();
  return 0;
}

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_HAS_PTHREADS)

#endif // AUTOBOOST_ASIO_DETAIL_IMPL_POSIX_THREAD_IPP
