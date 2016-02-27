// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <future>
#include <stdexcept>

namespace autowiring {
  /// <summary>
  /// Platform-specific operation appending routine
  /// </summary>
  /// <param name="future">The future to be appended to</param>
  /// <param name="fn">The lambda to be executed after the future is ready</param>
  template<typename T, typename Fn>
  void then(std::future<T>& future, Fn&& fn) {
    
  }
}
