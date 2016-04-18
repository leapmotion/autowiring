//
// detail/handler_type_requirements.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_HANDLER_TYPE_REQUIREMENTS_HPP
#define AUTOBOOST_ASIO_DETAIL_HANDLER_TYPE_REQUIREMENTS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

// Older versions of gcc have difficulty compiling the sizeof expressions where
// we test the handler type requirements. We'll disable checking of handler type
// requirements for those compilers, but otherwise enable it by default.
#if !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS)
# if !defined(__GNUC__) || (__GNUC__ >= 4)
#  define AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS 1
# endif // !defined(__GNUC__) || (__GNUC__ >= 4)
#endif // !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS)

// With C++0x we can use a combination of enhanced SFINAE and static_assert to
// generate better template error messages. As this technique is not yet widely
// portable, we'll only enable it for tested compilers.
#if !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)
# if defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT 1
#   endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#  endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
# endif // defined(__GNUC__)
# if defined(AUTOBOOST_ASIO_MSVC)
#  if (_MSC_VER >= 1600)
#   define AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT 1
#  endif // (_MSC_VER >= 1600)
# endif // defined(AUTOBOOST_ASIO_MSVC)
# if defined(__clang__)
#  if __has_feature(__cxx_static_assert__)
#   define AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT 1
#  endif // __has_feature(cxx_static_assert)
# endif // defined(__clang__)
#endif // !defined(AUTOBOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS)

#if defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)
# include <autoboost/asio/handler_type.hpp>
#endif // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)

// Newer gcc, clang need special treatment to suppress unused typedef warnings.
#if defined(__clang__)
# if defined(__apple_build_version__)
#  if (__clang_major__ >= 7)
#   define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
#  endif // (__clang_major__ >= 7)
# elif ((__clang_major__ == 3) && (__clang_minor__ >= 6)) \
    || (__clang_major__ > 3)
#  define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
# endif // ((__clang_major__ == 3) && (__clang_minor__ >= 6))
        //   || (__clang_major__ > 3)
#elif defined(__GNUC__)
# if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4)
#  define AUTOBOOST_ASIO_UNUSED_TYPEDEF __attribute__((__unused__))
# endif // ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ > 4)
#endif // defined(__GNUC__)
#if !defined(AUTOBOOST_ASIO_UNUSED_TYPEDEF)
# define AUTOBOOST_ASIO_UNUSED_TYPEDEF
#endif // !defined(AUTOBOOST_ASIO_UNUSED_TYPEDEF)

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)

# if defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)

template <typename Handler>
auto zero_arg_handler_test(Handler h, void*)
  -> decltype(
    sizeof(Handler(static_cast<const Handler&>(h))),
    ((h)()),
    char(0));

template <typename Handler>
char (&zero_arg_handler_test(Handler, ...))[2];

template <typename Handler, typename Arg1>
auto one_arg_handler_test(Handler h, Arg1* a1)
  -> decltype(
    sizeof(Handler(static_cast<const Handler&>(h))),
    ((h)(*a1)),
    char(0));

template <typename Handler>
char (&one_arg_handler_test(Handler h, ...))[2];

template <typename Handler, typename Arg1, typename Arg2>
auto two_arg_handler_test(Handler h, Arg1* a1, Arg2* a2)
  -> decltype(
    sizeof(Handler(static_cast<const Handler&>(h))),
    ((h)(*a1, *a2)),
    char(0));

template <typename Handler>
char (&two_arg_handler_test(Handler, ...))[2];

#  define AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT(expr, msg) \
     static_assert(expr, msg);

# else // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)

#  define AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT(expr, msg)

# endif // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)

template <typename T> T& lvref();
template <typename T> T& lvref(T);
template <typename T> const T& clvref();
template <typename T> const T& clvref(T);
template <typename T> char argbyv(T);

template <int>
struct handler_type_requirements
{
};

#define AUTOBOOST_ASIO_COMPLETION_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void()) asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::zero_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), 0)) == 1, \
      "CompletionHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()(), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_READ_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "ReadHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
            autoboost::asio::detail::lvref<const std::size_t>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF


#define AUTOBOOST_ASIO_WRITE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "WriteHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
            autoboost::asio::detail::lvref<const std::size_t>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_ACCEPT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::one_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0))) == 1, \
      "AcceptHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_CONNECT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::one_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0))) == 1, \
      "ConnectHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_COMPOSED_CONNECT_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, iter_type)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const iter_type*>(0))) == 1, \
      "ComposedConnectHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
            autoboost::asio::detail::lvref<const iter_type>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_RESOLVE_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, iter_type)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const iter_type*>(0))) == 1, \
      "ResolveHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
            autoboost::asio::detail::lvref<const iter_type>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_WAIT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::one_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0))) == 1, \
      "WaitHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_SIGNAL_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, int)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const int*>(0))) == 1, \
      "SignalHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
            autoboost::asio::detail::lvref<const int>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::one_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0))) == 1, \
      "HandshakeHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_BUFFERED_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::two_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "BufferedHandshakeHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
          autoboost::asio::detail::lvref<const autoboost::system::error_code>(), \
          autoboost::asio::detail::lvref<const std::size_t>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_SHUTDOWN_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef AUTOBOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(autoboost::system::error_code)) \
    asio_true_handler_type; \
  \
  AUTOBOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(autoboost::asio::detail::one_arg_handler_test( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const autoboost::system::error_code*>(0))) == 1, \
      "ShutdownHandler type requirements not met") \
  \
  typedef autoboost::asio::detail::handler_type_requirements< \
      sizeof( \
        autoboost::asio::detail::argbyv( \
          autoboost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        autoboost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            autoboost::asio::detail::lvref<const autoboost::system::error_code>()), \
        char(0))> AUTOBOOST_ASIO_UNUSED_TYPEDEF

#else // !defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)

#define AUTOBOOST_ASIO_COMPLETION_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_READ_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_WRITE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_ACCEPT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_CONNECT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_COMPOSED_CONNECT_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_RESOLVE_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_WAIT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_SIGNAL_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_BUFFERED_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#define AUTOBOOST_ASIO_SHUTDOWN_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int AUTOBOOST_ASIO_UNUSED_TYPEDEF

#endif // !defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)

} // namespace detail
} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_HANDLER_TYPE_REQUIREMENTS_HPP
