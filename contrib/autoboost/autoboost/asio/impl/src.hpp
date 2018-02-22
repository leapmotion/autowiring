//
// impl/src.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_IMPL_SRC_HPP
#define AUTOBOOST_ASIO_IMPL_SRC_HPP

#define AUTOBOOST_ASIO_SOURCE

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# error Do not compile Asio library source with AUTOBOOST_ASIO_HEADER_ONLY defined
#endif

#include <autoboost/asio/impl/error.ipp>
#include <autoboost/asio/impl/handler_alloc_hook.ipp>
#include <autoboost/asio/impl/io_service.ipp>
#include <autoboost/asio/impl/serial_port_base.ipp>
#include <autoboost/asio/detail/impl/buffer_sequence_adapter.ipp>
#include <autoboost/asio/detail/impl/descriptor_ops.ipp>
#include <autoboost/asio/detail/impl/dev_poll_reactor.ipp>
#include <autoboost/asio/detail/impl/epoll_reactor.ipp>
#include <autoboost/asio/detail/impl/eventfd_select_interrupter.ipp>
#include <autoboost/asio/detail/impl/handler_tracking.ipp>
#include <autoboost/asio/detail/impl/kqueue_reactor.ipp>
#include <autoboost/asio/detail/impl/pipe_select_interrupter.ipp>
#include <autoboost/asio/detail/impl/posix_event.ipp>
#include <autoboost/asio/detail/impl/posix_mutex.ipp>
#include <autoboost/asio/detail/impl/posix_thread.ipp>
#include <autoboost/asio/detail/impl/posix_tss_ptr.ipp>
#include <autoboost/asio/detail/impl/reactive_descriptor_service.ipp>
#include <autoboost/asio/detail/impl/reactive_serial_port_service.ipp>
#include <autoboost/asio/detail/impl/reactive_socket_service_base.ipp>
#include <autoboost/asio/detail/impl/resolver_service_base.ipp>
#include <autoboost/asio/detail/impl/select_reactor.ipp>
#include <autoboost/asio/detail/impl/service_registry.ipp>
#include <autoboost/asio/detail/impl/signal_set_service.ipp>
#include <autoboost/asio/detail/impl/socket_ops.ipp>
#include <autoboost/asio/detail/impl/socket_select_interrupter.ipp>
#include <autoboost/asio/detail/impl/strand_service.ipp>
#include <autoboost/asio/detail/impl/task_io_service.ipp>
#include <autoboost/asio/detail/impl/throw_error.ipp>
#include <autoboost/asio/detail/impl/timer_queue_ptime.ipp>
#include <autoboost/asio/detail/impl/timer_queue_set.ipp>
#include <autoboost/asio/detail/impl/win_iocp_handle_service.ipp>
#include <autoboost/asio/detail/impl/win_iocp_io_service.ipp>
#include <autoboost/asio/detail/impl/win_iocp_serial_port_service.ipp>
#include <autoboost/asio/detail/impl/win_iocp_socket_service_base.ipp>
#include <autoboost/asio/detail/impl/win_event.ipp>
#include <autoboost/asio/detail/impl/win_mutex.ipp>
#include <autoboost/asio/detail/impl/win_object_handle_service.ipp>
#include <autoboost/asio/detail/impl/win_static_mutex.ipp>
#include <autoboost/asio/detail/impl/win_thread.ipp>
#include <autoboost/asio/detail/impl/win_tss_ptr.ipp>
#include <autoboost/asio/detail/impl/winrt_ssocket_service_base.ipp>
#include <autoboost/asio/detail/impl/winrt_timer_scheduler.ipp>
#include <autoboost/asio/detail/impl/winsock_init.ipp>
#include <autoboost/asio/generic/detail/impl/endpoint.ipp>
#include <autoboost/asio/ip/impl/address.ipp>
#include <autoboost/asio/ip/impl/address_v4.ipp>
#include <autoboost/asio/ip/impl/address_v6.ipp>
#include <autoboost/asio/ip/impl/host_name.ipp>
#include <autoboost/asio/ip/detail/impl/endpoint.ipp>
#include <autoboost/asio/local/detail/impl/endpoint.ipp>

#endif // AUTOBOOST_ASIO_IMPL_SRC_HPP
