// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include MEMORY_HEADER
#include TYPE_TRAITS_HEADER

namespace autowiring {
  /// <summary>
  /// Identical to static_pointer_cast if U inherits T, dynamic_pointer_cast otherwise
  /// </summary>
  template<class T, class U>
  typename std::enable_if<
    std::is_base_of<T, U>::value && !std::is_same<T, U>::value,
    typename std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>& Other) {
    return std::static_pointer_cast<T, U>(Other);
  };

  template<class T, class U>
  typename std::enable_if<
    !std::is_base_of<T, U>::value &&
    std::is_polymorphic<U>::value &&
    !std::is_scalar<T>::value,
    std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>& Other) {
    return std::dynamic_pointer_cast<T, U>(Other);
  }

  template<class T, class U>
  typename std::enable_if<
    (
      !std::is_polymorphic<U>::value ||
      std::is_scalar<T>::value
    ) && !std::is_same<T, U>::value,
    std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>&) {
    return std::shared_ptr<T>();
  }

  template<class T, class U>
  typename std::enable_if<
    std::is_same<T, U>::value,
    std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>& ptr) {
    return ptr;
  }
}