// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2013 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_COUNTER_HPP
#define AUTOBOOST_THREAD_COUNTER_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>

//#include <autoboost/thread/mutex.hpp>
//#include <autoboost/thread/lock_types.hpp>
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/chrono/duration.hpp>
#include <autoboost/chrono/time_point.hpp>
#include <autoboost/assert.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
  namespace detail {
    struct counter
    {
      condition_variable cond_;
      std::size_t value_;

      counter(std::size_t value)
      : value_(value)
      {

      }
      counter& operator=(counter const& rhs)
      {
        value_ = rhs.value_;
        return *this;
      }
      counter& operator=(std::size_t value)
      {
        value_ = value;
        return *this;
      }

      operator std::size_t() const
      {
        return value_;
      }
      operator std::size_t&()
      {
        return value_;
      }

      void inc_and_notify_all()
      {
        ++value_;
        cond_.notify_all();
      }

      void dec_and_notify_all()
      {
        --value_;
        cond_.notify_all();
      }
      void assign_and_notify_all(counter const& rhs)
      {
        value_ = rhs.value_;
        cond_.notify_all();
      }
      void assign_and_notify_all(std::size_t value)
      {
        value_ = value;
        cond_.notify_all();
      }
    };
    struct counter_is_not_zero
    {
      counter_is_not_zero(counter const& count) : count_(count) {}
      bool operator()() const { return count_ != 0; }
      counter const& count_;
    };
    struct counter_is_zero
    {
      counter_is_zero(counter const& count) : count_(count) {}
      bool operator()() const { return count_ == 0; }
      counter const& count_;
    };
    struct is_zero
    {
      is_zero(std::size_t& count) : count_(count) {}
      bool operator()() const { return count_ == 0; }
      std::size_t& count_;
    };
    struct not_equal
    {
      not_equal(std::size_t& x, std::size_t& y) : x_(x), y_(y) {}
      bool operator()() const { return x_ != y_; }
      std::size_t& x_;
      std::size_t& y_;
    };
  }
} // namespace autoboost

#include <autoboost/config/abi_suffix.hpp>

#endif
