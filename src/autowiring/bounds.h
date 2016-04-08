// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <type_traits>

namespace autowiring {
  /// <summary>
  /// Configuration metadata field used to specify lower and upper bounds
  /// </summary>
  /// <remarks>
  /// This field is for integral types, use bounds_f for floating-point types
  /// </remarks>
  template<typename T>
  struct bounds
  {
    typedef T type;

    // Holds true if we allow this instantiation _for a particular member variable_
    template<typename U>
    struct valid {
      static const bool value = std::is_arithmetic<T>::value;
    };

    // Only one of these allowed per field
    static const bool multi = false;

    bounds(void) = default;
    bounds(T&& min, T&& max) :
      min(min),
      max(max)
    {}

    T min;
    T max;
    T* pValue;
  };
}
