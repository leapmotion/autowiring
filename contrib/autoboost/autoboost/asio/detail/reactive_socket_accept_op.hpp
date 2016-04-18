//
// detail/reactive_socket_accept_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_REACTIVE_SOCKET_ACCEPT_OP_HPP
#define AUTOBOOST_ASIO_DETAIL_REACTIVE_SOCKET_ACCEPT_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>
#include <autoboost/asio/detail/addressof.hpp>
#include <autoboost/asio/detail/bind_handler.hpp>
#include <autoboost/asio/detail/buffer_sequence_adapter.hpp>
#include <autoboost/asio/detail/fenced_block.hpp>
#include <autoboost/asio/detail/reactor_op.hpp>
#include <autoboost/asio/detail/socket_holder.hpp>
#include <autoboost/asio/detail/socket_ops.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

template <typename Socket, typename Protocol>
class reactive_socket_accept_op_base : public reactor_op
{
public:
  reactive_socket_accept_op_base(socket_type socket,
      socket_ops::state_type state, Socket& peer, const Protocol& protocol,
      typename Protocol::endpoint* peer_endpoint, func_type complete_func)
    : reactor_op(&reactive_socket_accept_op_base::do_perform, complete_func),
      socket_(socket),
      state_(state),
      peer_(peer),
      protocol_(protocol),
      peer_endpoint_(peer_endpoint)
  {
  }

  static bool do_perform(reactor_op* base)
  {
    reactive_socket_accept_op_base* o(
        static_cast<reactive_socket_accept_op_base*>(base));

    std::size_t addrlen = o->peer_endpoint_ ? o->peer_endpoint_->capacity() : 0;
    socket_type new_socket = invalid_socket;
    bool result = socket_ops::non_blocking_accept(o->socket_,
          o->state_, o->peer_endpoint_ ? o->peer_endpoint_->data() : 0,
          o->peer_endpoint_ ? &addrlen : 0, o->ec_, new_socket);

    // On success, assign new connection to peer socket object.
    if (new_socket != invalid_socket)
    {
      socket_holder new_socket_holder(new_socket);
      if (o->peer_endpoint_)
        o->peer_endpoint_->resize(addrlen);
      if (!o->peer_.assign(o->protocol_, new_socket, o->ec_))
        new_socket_holder.release();
    }

    return result;
  }

private:
  socket_type socket_;
  socket_ops::state_type state_;
  Socket& peer_;
  Protocol protocol_;
  typename Protocol::endpoint* peer_endpoint_;
};

template <typename Socket, typename Protocol, typename Handler>
class reactive_socket_accept_op :
  public reactive_socket_accept_op_base<Socket, Protocol>
{
public:
  AUTOBOOST_ASIO_DEFINE_HANDLER_PTR(reactive_socket_accept_op);

  reactive_socket_accept_op(socket_type socket,
      socket_ops::state_type state, Socket& peer, const Protocol& protocol,
      typename Protocol::endpoint* peer_endpoint, Handler& handler)
    : reactive_socket_accept_op_base<Socket, Protocol>(socket, state, peer,
        protocol, peer_endpoint, &reactive_socket_accept_op::do_complete),
      handler_(AUTOBOOST_ASIO_MOVE_CAST(Handler)(handler))
  {
  }

  static void do_complete(io_service_impl* owner, operation* base,
      const autoboost::system::error_code& /*ec*/,
      std::size_t /*bytes_transferred*/)
  {
    // Take ownership of the handler object.
    reactive_socket_accept_op* o(static_cast<reactive_socket_accept_op*>(base));
    ptr p = { autoboost::asio::detail::addressof(o->handler_), o, o };

    AUTOBOOST_ASIO_HANDLER_COMPLETION((o));

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder1<Handler, autoboost::system::error_code>
      handler(o->handler_, o->ec_);
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

#endif // AUTOBOOST_ASIO_DETAIL_REACTIVE_SOCKET_ACCEPT_OP_HPP
