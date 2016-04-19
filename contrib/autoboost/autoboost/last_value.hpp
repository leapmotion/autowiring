// last_value function object (documented as part of Boost.Signals)

// Copyright Douglas Gregor 2001-2003. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/signals

#ifndef AUTOBOOST_LAST_VALUE_HPP
#define AUTOBOOST_LAST_VALUE_HPP

#include <cassert>
#include <autoboost/config.hpp>

namespace autoboost {
  template<typename T>
  struct last_value {
    typedef T result_type;

    template<typename InputIterator>
    T operator()(InputIterator first, InputIterator last) const
    {
      assert(first != last);
      T value = *first++;
      while (first != last)
        value = *first++;
      return value;
    }
  };

  template<>
  struct last_value<void> {
#ifdef AUTOBOOST_NO_VOID_RETURNS
    struct unusable {};

  public:
    typedef unusable result_type;
#else
  public:
    typedef void result_type;
#endif // AUTOBOOST_NO_VOID_RETURNS

    template<typename InputIterator>
    result_type
    operator()(InputIterator first, InputIterator last) const
    {
      while (first != last)
        *first++;
      return result_type();
    }
  };
}
#endif // AUTOBOOST_SIGNALS_LAST_VALUE_HPP
