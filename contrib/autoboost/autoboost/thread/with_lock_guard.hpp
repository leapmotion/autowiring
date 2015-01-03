// (C) Copyright 2013 Ruslan Baratov
// Copyright (C) 2014 Vicente J. Botet Escriba
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See www.boost.org/libs/thread for documentation.

#ifndef AUTOBOOST_THREAD_WITH_LOCK_GUARD_HPP
#define AUTOBOOST_THREAD_WITH_LOCK_GUARD_HPP

#include <autoboost/thread/lock_guard.hpp>
#include <autoboost/utility/result_of.hpp>
//#include <autoboost/thread/detail/invoke.hpp>

namespace autoboost {

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && \
    !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && \
    !defined(AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES)

/**
 * Utility to run functions in scope protected by mutex.
 *
 * Examples:
 *
 *     int func(int, int&);
 *     autoboost::mutex m;
 *     int a;
 *     int result = autoboost::with_lock_guard(m, func, 1, autoboost::ref(a));
 *
 *     // using autoboost::bind
 *     int result = autoboost::with_lock_guard(
 *         m, autoboost::bind(func, 2, autoboost::ref(a))
 *     );
 *
 *     // using lambda
 *     int a;
 *     int result = autoboost::with_lock_guard(
 *         m,
 *         [&a](int x) {
 *           a = 3;
 *           return x + 4;
 *         },
 *         5
 *     );
 */
template <class Lockable, class Function, class... Args>
typename autoboost::result_of<Function(Args...)>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Function) func,
    AUTOBOOST_FWD_REF(Args)... args
) //-> decltype(func(autoboost::forward<Args>(args)...))
{
  autoboost::lock_guard<Lockable> lock(m);
  return func(autoboost::forward<Args>(args)...);
}

#else

// Workaround versions for compilers without c++11 variadic templates support.
// (function arguments limit: 4)
// (for lambda support define AUTOBOOST_RESULT_OF_USE_DECLTYPE may be needed)

template <class Lockable, class Func>
typename autoboost::result_of<Func()>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Func) func
) {
  autoboost::lock_guard<Lockable> lock(m);
  return func();
}

template <class Lockable, class Func, class Arg>
typename autoboost::result_of<Func(Arg)>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Func) func,
    AUTOBOOST_FWD_REF(Arg) arg
) {
  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg>(arg)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2>
typename autoboost::result_of<Func(Arg1, Arg2)>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Func) func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2
) {
  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2, class Arg3>
typename autoboost::result_of<Func(Arg1, Arg2, Arg3)>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Func) func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2,
    AUTOBOOST_FWD_REF(Arg3) arg3
) {
  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2),
      autoboost::forward<Arg3>(arg3)
  );
}

template <
    class Lockable, class Func, class Arg1, class Arg2, class Arg3, class Arg4
>
typename autoboost::result_of<Func(Arg1, Arg2, Arg3, Arg4)>::type with_lock_guard(
    Lockable& m,
    AUTOBOOST_FWD_REF(Func) func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2,
    AUTOBOOST_FWD_REF(Arg3) arg3,
    AUTOBOOST_FWD_REF(Arg4) arg4
) {
  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2),
      autoboost::forward<Arg3>(arg3),
      autoboost::forward<Arg4>(arg4)
  );
}

// overloads for function pointer
// (if argument is not function pointer, static assert will trigger)
template <class Lockable, class Func>
typename autoboost::result_of<
    typename autoboost::add_pointer<Func>::type()
>::type with_lock_guard(
    Lockable& m,
    Func* func
) {
  AUTOBOOST_STATIC_ASSERT(autoboost::is_function<Func>::value);

  autoboost::lock_guard<Lockable> lock(m);
  return func();
}

template <class Lockable, class Func, class Arg>
typename autoboost::result_of<
    typename autoboost::add_pointer<Func>::type(Arg)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    AUTOBOOST_FWD_REF(Arg) arg
) {
  AUTOBOOST_STATIC_ASSERT(autoboost::is_function<Func>::value);

  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg>(arg)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2>
typename autoboost::result_of<
    typename autoboost::add_pointer<Func>::type(Arg1, Arg2)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2
) {
  AUTOBOOST_STATIC_ASSERT(autoboost::is_function<Func>::value);

  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2, class Arg3>
typename autoboost::result_of<
    typename autoboost::add_pointer<Func>::type(Arg1, Arg2, Arg3)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2,
    AUTOBOOST_FWD_REF(Arg3) arg3
) {
  AUTOBOOST_STATIC_ASSERT(autoboost::is_function<Func>::value);

  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2),
      autoboost::forward<Arg3>(arg3)
  );
}

template <
    class Lockable, class Func, class Arg1, class Arg2, class Arg3, class Arg4
>
typename autoboost::result_of<
    typename autoboost::add_pointer<Func>::type(Arg1, Arg2, Arg3, Arg4)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    AUTOBOOST_FWD_REF(Arg1) arg1,
    AUTOBOOST_FWD_REF(Arg2) arg2,
    AUTOBOOST_FWD_REF(Arg3) arg3,
    AUTOBOOST_FWD_REF(Arg4) arg4
) {
  AUTOBOOST_STATIC_ASSERT(autoboost::is_function<Func>::value);

  autoboost::lock_guard<Lockable> lock(m);
  return func(
      autoboost::forward<Arg1>(arg1),
      autoboost::forward<Arg2>(arg2),
      autoboost::forward<Arg3>(arg3),
      autoboost::forward<Arg4>(arg4)
  );
}

#endif

} // namespace autoboost

#endif // AUTOBOOST_THREAD_WITH_LOCK_GUARD_HPP

