// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  /// <summary>
  /// Finds the specified type T in the argument pack
  /// </remarks>
  template<typename T, typename... Args>
  struct find {
    static const bool value = false;
    static const size_t index = 0;
  };

  template<typename T, typename Arg, typename... Args>
  struct find<T, Arg, Args...> {
    // Holds true if T is found, false otherwise
    static const bool value =
      std::is_same<T, Arg>::value ||
      find<T, Args...>::value;

    // Holds one more than the index of the found item.  In the case that multiple
    // matches are found, holds the sum of all of those values.
    static const size_t index =
      (value ? 1 + find<T, Args...>::index : 0);
  };
}