// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_LOCK_FACTORIES_HPP
#define AUTOBOOST_THREAD_LOCK_FACTORIES_HPP

#include <autoboost/thread/lock_types.hpp>
#include <autoboost/thread/lock_algorithms.hpp>
#if ! defined(AUTOBOOST_THREAD_NO_MAKE_UNIQUE_LOCKS)
#include <tuple> // todo change to <autoboost/tuple.hpp> once Boost.Tuple or Boost.Fusion provides Move semantics.
#endif
#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{

  template <typename Lockable>
  unique_lock<Lockable> make_unique_lock(Lockable& mtx)
  {
    return unique_lock<Lockable> (mtx);
  }

  template <typename Lockable>
  unique_lock<Lockable> make_unique_lock(Lockable& mtx, adopt_lock_t)
  {
    return unique_lock<Lockable> (mtx, adopt_lock);
  }

  template <typename Lockable>
  unique_lock<Lockable> make_unique_lock(Lockable& mtx, defer_lock_t)
  {
    return unique_lock<Lockable> (mtx, defer_lock);
  }

  template <typename Lockable>
  unique_lock<Lockable> make_unique_lock(Lockable& mtx, try_to_lock_t)
  {
    return unique_lock<Lockable> (mtx, try_to_lock);
  }
#if ! defined(AUTOBOOST_THREAD_NO_MAKE_UNIQUE_LOCKS)

#if ! defined AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
  template <typename ...Lockable>
  std::tuple<unique_lock<Lockable> ...> make_unique_locks(Lockable& ...mtx)
  {
    autoboost::lock(mtx...);
    return std::tuple<unique_lock<Lockable> ...>(unique_lock<Lockable>(mtx, adopt_lock)...);
  }
#else
  template <typename L1, typename L2>
  std::tuple<unique_lock<L1>, unique_lock<L2> > make_unique_locks(L1& m1, L2& m2)
  {
    autoboost::lock(m1, m2);
    return std::tuple<unique_lock<L1>,unique_lock<L2> >(
        unique_lock<L1>(m1, adopt_lock),
        unique_lock<L2>(m2, adopt_lock)
    );
  }
  template <typename L1, typename L2, typename L3>
  std::tuple<unique_lock<L1>, unique_lock<L2>, unique_lock<L3> > make_unique_locks(L1& m1, L2& m2, L3& m3)
  {
    autoboost::lock(m1, m2, m3);
    return std::tuple<unique_lock<L1>,unique_lock<L2>,unique_lock<L3> >(
        unique_lock<L1>(m1, adopt_lock),
        unique_lock<L2>(m2, adopt_lock),
        unique_lock<L3>(m3, adopt_lock)
    );
  }

#endif
#endif

}

#include <autoboost/config/abi_suffix.hpp>
#endif
