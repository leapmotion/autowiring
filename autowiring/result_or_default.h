// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

/// <param name="fn">The function to be called</param>
/// <param name="args">The arguments to be passed to the function</param>
/// <returns>
/// The result of fn if decltype(fn(args...)) is the same as T, otherwise default
/// </returns>
template<class T, class Fn, class... Args>
typename std::enable_if<
  std::is_same<
    T,
    typename std::result_of<Fn(Args...)>::type
  >::value,
  T
>::type
result_or_default(Fn& fn, T def, Args&&... args) {
  return fn(std::forward<Args>(args)...);
}

template<class T, class Fn, class... Args>
typename std::enable_if<
  !std::is_same<
    T,
    typename std::result_of<Fn(Args...)>::type
  >::value,
  T
>::type
result_or_default(Fn& fn, T def, Args&&... args) {
  fn(std::forward<Args>(args)...);
  return def;
}
