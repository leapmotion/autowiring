//
// detail/handler_tracking.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_HANDLER_TRACKING_HPP
#define AUTOBOOST_ASIO_DETAIL_HANDLER_TRACKING_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TRACKING)
# include <autoboost/system/error_code.hpp>
# include <autoboost/asio/detail/cstdint.hpp>
# include <autoboost/asio/detail/static_mutex.hpp>
# include <autoboost/asio/detail/tss_ptr.hpp>
#endif // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TRACKING)

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

#if defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TRACKING)

class handler_tracking
{
public:
  class completion;

  // Base class for objects containing tracked handlers.
  class tracked_handler
  {
  private:
    // Only the handler_tracking class will have access to the id.
    friend class handler_tracking;
    friend class completion;
    uint64_t id_;

  protected:
    // Constructor initialises with no id.
    tracked_handler() : id_(0) {}

    // Prevent deletion through this type.
    ~tracked_handler() {}
  };

  // Initialise the tracking system.
  AUTOBOOST_ASIO_DECL static void init();

  // Record the creation of a tracked handler.
  AUTOBOOST_ASIO_DECL static void creation(tracked_handler* h,
      const char* object_type, void* object, const char* op_name);

  class completion
  {
  public:
    // Constructor records that handler is to be invoked with no arguments.
    AUTOBOOST_ASIO_DECL explicit completion(tracked_handler* h);

    // Destructor records only when an exception is thrown from the handler, or
    // if the memory is being freed without the handler having been invoked.
    AUTOBOOST_ASIO_DECL ~completion();

    // Records that handler is to be invoked with no arguments.
    AUTOBOOST_ASIO_DECL void invocation_begin();

    // Records that handler is to be invoked with one arguments.
    AUTOBOOST_ASIO_DECL void invocation_begin(const autoboost::system::error_code& ec);

    // Constructor records that handler is to be invoked with two arguments.
    AUTOBOOST_ASIO_DECL void invocation_begin(
        const autoboost::system::error_code& ec, std::size_t bytes_transferred);

    // Constructor records that handler is to be invoked with two arguments.
    AUTOBOOST_ASIO_DECL void invocation_begin(
        const autoboost::system::error_code& ec, int signal_number);

    // Constructor records that handler is to be invoked with two arguments.
    AUTOBOOST_ASIO_DECL void invocation_begin(
        const autoboost::system::error_code& ec, const char* arg);

    // Record that handler invocation has ended.
    AUTOBOOST_ASIO_DECL void invocation_end();

  private:
    friend class handler_tracking;
    uint64_t id_;
    bool invoked_;
    completion* next_;
  };

  // Record an operation that affects pending handlers.
  AUTOBOOST_ASIO_DECL static void operation(const char* object_type,
      void* object, const char* op_name);

  // Write a line of output.
  AUTOBOOST_ASIO_DECL static void write_line(const char* format, ...);

private:
  struct tracking_state;
  AUTOBOOST_ASIO_DECL static tracking_state* get_state();
};

# define AUTOBOOST_ASIO_INHERIT_TRACKED_HANDLER \
  : public autoboost::asio::detail::handler_tracking::tracked_handler

# define AUTOBOOST_ASIO_ALSO_INHERIT_TRACKED_HANDLER \
  , public autoboost::asio::detail::handler_tracking::tracked_handler

# define AUTOBOOST_ASIO_HANDLER_TRACKING_INIT \
  autoboost::asio::detail::handler_tracking::init()

# define AUTOBOOST_ASIO_HANDLER_CREATION(args) \
  autoboost::asio::detail::handler_tracking::creation args

# define AUTOBOOST_ASIO_HANDLER_COMPLETION(args) \
  autoboost::asio::detail::handler_tracking::completion tracked_completion args

# define AUTOBOOST_ASIO_HANDLER_INVOCATION_BEGIN(args) \
  tracked_completion.invocation_begin args

# define AUTOBOOST_ASIO_HANDLER_INVOCATION_END \
  tracked_completion.invocation_end()

# define AUTOBOOST_ASIO_HANDLER_OPERATION(args) \
  autoboost::asio::detail::handler_tracking::operation args

#else // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TRACKING)

# define AUTOBOOST_ASIO_INHERIT_TRACKED_HANDLER
# define AUTOBOOST_ASIO_ALSO_INHERIT_TRACKED_HANDLER
# define AUTOBOOST_ASIO_HANDLER_TRACKING_INIT (void)0
# define AUTOBOOST_ASIO_HANDLER_CREATION(args) (void)0
# define AUTOBOOST_ASIO_HANDLER_COMPLETION(args) (void)0
# define AUTOBOOST_ASIO_HANDLER_INVOCATION_BEGIN(args) (void)0
# define AUTOBOOST_ASIO_HANDLER_INVOCATION_END (void)0
# define AUTOBOOST_ASIO_HANDLER_OPERATION(args) (void)0

#endif // defined(AUTOBOOST_ASIO_ENABLE_HANDLER_TRACKING)

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/handler_tracking.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // AUTOBOOST_ASIO_DETAIL_HANDLER_TRACKING_HPP
