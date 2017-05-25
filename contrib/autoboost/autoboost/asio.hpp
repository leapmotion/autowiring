//
// asio.hpp
// ~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  See www.boost.org/libs/asio for documentation.
//

#ifndef AUTOBOOST_ASIO_HPP
#define AUTOBOOST_ASIO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/async_result.hpp>
#include <autoboost/asio/basic_datagram_socket.hpp>
#include <autoboost/asio/basic_deadline_timer.hpp>
#include <autoboost/asio/basic_io_object.hpp>
#include <autoboost/asio/basic_raw_socket.hpp>
#include <autoboost/asio/basic_seq_packet_socket.hpp>
#include <autoboost/asio/basic_serial_port.hpp>
#include <autoboost/asio/basic_signal_set.hpp>
#include <autoboost/asio/basic_socket_acceptor.hpp>
#include <autoboost/asio/basic_socket_iostream.hpp>
#include <autoboost/asio/basic_socket_streambuf.hpp>
#include <autoboost/asio/basic_stream_socket.hpp>
#include <autoboost/asio/basic_streambuf.hpp>
#include <autoboost/asio/basic_waitable_timer.hpp>
#include <autoboost/asio/buffer.hpp>
#include <autoboost/asio/buffered_read_stream_fwd.hpp>
#include <autoboost/asio/buffered_read_stream.hpp>
#include <autoboost/asio/buffered_stream_fwd.hpp>
#include <autoboost/asio/buffered_stream.hpp>
#include <autoboost/asio/buffered_write_stream_fwd.hpp>
#include <autoboost/asio/buffered_write_stream.hpp>
#include <autoboost/asio/buffers_iterator.hpp>
#include <autoboost/asio/completion_condition.hpp>
#include <autoboost/asio/connect.hpp>
#include <autoboost/asio/coroutine.hpp>
#include <autoboost/asio/datagram_socket_service.hpp>
#include <autoboost/asio/deadline_timer_service.hpp>
#include <autoboost/asio/deadline_timer.hpp>
#include <autoboost/asio/error.hpp>
#include <autoboost/asio/generic/basic_endpoint.hpp>
#include <autoboost/asio/generic/datagram_protocol.hpp>
#include <autoboost/asio/generic/raw_protocol.hpp>
#include <autoboost/asio/generic/seq_packet_protocol.hpp>
#include <autoboost/asio/generic/stream_protocol.hpp>
#include <autoboost/asio/handler_alloc_hook.hpp>
#include <autoboost/asio/handler_continuation_hook.hpp>
#include <autoboost/asio/handler_invoke_hook.hpp>
#include <autoboost/asio/handler_type.hpp>
#include <autoboost/asio/io_service.hpp>
#include <autoboost/asio/ip/address.hpp>
#include <autoboost/asio/ip/address_v4.hpp>
#include <autoboost/asio/ip/address_v6.hpp>
#include <autoboost/asio/ip/basic_endpoint.hpp>
#include <autoboost/asio/ip/basic_resolver.hpp>
#include <autoboost/asio/ip/basic_resolver_entry.hpp>
#include <autoboost/asio/ip/basic_resolver_iterator.hpp>
#include <autoboost/asio/ip/basic_resolver_query.hpp>
#include <autoboost/asio/ip/host_name.hpp>
#include <autoboost/asio/ip/icmp.hpp>
#include <autoboost/asio/ip/multicast.hpp>
#include <autoboost/asio/ip/resolver_query_base.hpp>
#include <autoboost/asio/ip/resolver_service.hpp>
#include <autoboost/asio/ip/tcp.hpp>
#include <autoboost/asio/ip/udp.hpp>
#include <autoboost/asio/ip/unicast.hpp>
#include <autoboost/asio/ip/v6_only.hpp>
#include <autoboost/asio/is_read_buffered.hpp>
#include <autoboost/asio/is_write_buffered.hpp>
#include <autoboost/asio/local/basic_endpoint.hpp>
#include <autoboost/asio/local/connect_pair.hpp>
#include <autoboost/asio/local/datagram_protocol.hpp>
#include <autoboost/asio/local/stream_protocol.hpp>
#include <autoboost/asio/placeholders.hpp>
#include <autoboost/asio/posix/basic_descriptor.hpp>
#include <autoboost/asio/posix/basic_stream_descriptor.hpp>
#include <autoboost/asio/posix/descriptor_base.hpp>
#include <autoboost/asio/posix/stream_descriptor.hpp>
#include <autoboost/asio/posix/stream_descriptor_service.hpp>
#include <autoboost/asio/raw_socket_service.hpp>
#include <autoboost/asio/read.hpp>
#include <autoboost/asio/read_at.hpp>
#include <autoboost/asio/read_until.hpp>
#include <autoboost/asio/seq_packet_socket_service.hpp>
#include <autoboost/asio/serial_port.hpp>
#include <autoboost/asio/serial_port_base.hpp>
#include <autoboost/asio/serial_port_service.hpp>
#include <autoboost/asio/signal_set.hpp>
#include <autoboost/asio/signal_set_service.hpp>
#include <autoboost/asio/socket_acceptor_service.hpp>
#include <autoboost/asio/socket_base.hpp>
#include <autoboost/asio/strand.hpp>
#include <autoboost/asio/stream_socket_service.hpp>
#include <autoboost/asio/streambuf.hpp>
#include <autoboost/asio/time_traits.hpp>
#include <autoboost/asio/version.hpp>
#include <autoboost/asio/wait_traits.hpp>
#include <autoboost/asio/waitable_timer_service.hpp>
#include <autoboost/asio/windows/basic_handle.hpp>
#include <autoboost/asio/windows/basic_object_handle.hpp>
#include <autoboost/asio/windows/basic_random_access_handle.hpp>
#include <autoboost/asio/windows/basic_stream_handle.hpp>
#include <autoboost/asio/windows/object_handle.hpp>
#include <autoboost/asio/windows/object_handle_service.hpp>
#include <autoboost/asio/windows/overlapped_ptr.hpp>
#include <autoboost/asio/windows/random_access_handle.hpp>
#include <autoboost/asio/windows/random_access_handle_service.hpp>
#include <autoboost/asio/windows/stream_handle.hpp>
#include <autoboost/asio/windows/stream_handle_service.hpp>
#include <autoboost/asio/write.hpp>
#include <autoboost/asio/write_at.hpp>

#endif // AUTOBOOST_ASIO_HPP
