//
// impl/ssl/src.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_SSL_IMPL_SRC_HPP
#define AUTOBOOST_ASIO_SSL_IMPL_SRC_HPP

#define AUTOBOOST_ASIO_SOURCE

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# error Do not compile Asio library source with AUTOBOOST_ASIO_HEADER_ONLY defined
#endif

#include <autoboost/asio/ssl/impl/context.ipp>
#include <autoboost/asio/ssl/impl/error.ipp>
#include <autoboost/asio/ssl/detail/impl/engine.ipp>
#include <autoboost/asio/ssl/detail/impl/openssl_init.ipp>
#include <autoboost/asio/ssl/impl/rfc2818_verification.ipp>

#endif // AUTOBOOST_ASIO_SSL_IMPL_SRC_HPP
