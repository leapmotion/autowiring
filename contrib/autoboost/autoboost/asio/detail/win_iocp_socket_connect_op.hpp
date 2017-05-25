//
// detail/win_iocp_socket_connect_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WIN_IOCP_SOCKET_CONNECT_OP_HPP
#define AUTOBOOST_ASIO_DETAIL_WIN_IOCP_SOCKET_CONNECT_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_IOCP)

#include <autoboost/asio/detail/addressof.hpp>
#include <autoboost/asio/detail/bind_handler.hpp>
#include <autoboost/asio/detail/fenced_block.hpp>
#include <autoboost/asio/detail/handler_alloc_helpers.hpp>
#include <autoboost/asio/detail/handler_invoke_helpers.hpp>
#include <autoboost/asio/detail/reactor_op.hpp>
#include <autoboost/asio/detail/socket_ops.hpp>
#include <autoboost/asio/error.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class win_iocp_socket_connect_op_base : public reactor_op
{
public:
  win_iocp_socket_connect_op_base(socket_type socket, func_type complete_func)
    : reactor_op(&win_iocp_socket_connect_op_base::do_perform, complete_func),
      socket_(socket),
      connect_ex_(false)
  {
  }

  static bool do_perform(reactor_op* base)
  {
    win_iocp_socket_connect_op_base* o(
        static_cast<win_iocp_socket_connect_op_base*>(base));

    return socket_ops::non_blocking_connect(o->socket_, o->ec_);
  }

  socket_type socket_;
  bool connect_ex_;
};

template <typename Handler>
class win_iocp_socket_connect_op : public win_iocp_socket_connect_op_base
{
public:
  AUTOBOOST_ASIO_DEFINE_HANDLER_PTR(win_iocp_socket_connect_op);

  win_iocp_socket_connect_op(socket_type socket, Handler& handler)
    : win_iocp_socket_connect_op_base(socket,
        &win_iocp_socket_connect_op::do_complete),
      handler_(AUTOBOOST_ASIO_MOVE_CAST(Handler)(handler))
  {
  }

  static void do_complete(io_service_impl* owner, operation* base,
      const autoboost::system::error_code& result_ec,
      std::size_t /*bytes_transferred*/)
  {
    autoboost::system::error_code ec(result_ec);

    // Take ownership of the operation object.
    win_iocp_socket_connect_op* o(
        static_cast<win_iocp_socket_connect_op*>(base));
    ptr p = { autoboost::asio::detail::addressof(o->handler_), o, o };

    if (owner)
    {
      if (o->connect_ex_)
        socket_ops::complete_iocp_connect(o->socket_, ec);
      else
        ec = o->ec_;
    }

    AUTOBOOST_ASIO_HANDLER_COMPLETION((o));

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder1<Handler, autoboost::system::error_code>
      handler(o->handler_, ec);
    p.h = autoboost::asio::detail::addressof(handler.handler_);
    p.reset();

    // Make the upcall if required.
    if (owner)
    {
      fenced_block b(fenced_block::half);
      AUTOBOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_));
      autoboost_asio_handler_invoke_helpers::invoke(handler, handler.handler_);
      AUTOBOOST_ASIO_HANDLER_INVOCATION_END;
    }
  }

private:
  Handler handler_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)

#endif // AUTOBOOST_ASIO_DETAIL_WIN_IOCP_SOCKET_CONNECT_OP_HPP
