// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include MEMORY_HEADER
#include TYPE_TRAITS_HEADER

namespace autowiring {
  template<class T, class U>
  struct fast_pointer_cast_blind;

  template<class T, class U>
  struct fast_pointer_cast_initializer;

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
    std::is_class<T>::value,
    std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>& Other) {
    return std::dynamic_pointer_cast<T, U>(Other);
  }

  template<class T, class U>
  typename std::enable_if<
    (
      !std::is_polymorphic<U>::value ||
      !std::is_class<T>::value
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

  /// <summary>
  /// Holds a fast pointer cast function pointer
  /// </summary>
  /// <remarks>
  /// This method holds the implementation of the blind fast pointer cast function
  /// </summary>
  template<class T, class U>
  struct fast_pointer_cast_blind {
    /// <summary>
    /// A delayed instantiation fast pointer cast
    /// </summary>
    /// <remarks>
    /// This method will perform a correct fast pointer cast to the destination type without _any_ knowledge
    /// of either T or U being required.  It's able to do this as long as there is another location somewhere
    /// in the application where a fast_pointer_cast is being used, because the fast_pointer_cast type has a
    /// small registry of cast functions.
    /// </remarks>
    static std::shared_ptr<T>(*cast)(const std::shared_ptr<U>&);
  };

  /// <summary>
  /// Trivial case specialization
  /// </summary>
  template<class T>
  struct fast_pointer_cast_blind<T, T> {
    static std::shared_ptr<T> cast(const std::shared_ptr<T>& rhs) {
      return rhs;
    }
  };

  /// <summary>
  /// Null implementation of the fast pointer cast function
  /// </summary>
  template<class T, class U>
  std::shared_ptr<T> null_cast(const std::shared_ptr<U>&) {
    return nullptr;
  }

  // Default cast routine is going to be a do-nothing function
  template<class T, class U>
  std::shared_ptr<T>(*fast_pointer_cast_blind<T, U>::cast)(const std::shared_ptr<U>&) = &null_cast<T, U>;

  /// <summary>
  /// Initializer for the fast pointer cast holder
  /// </summary>
  template<class T, class U>
  struct fast_pointer_cast_initializer {
    fast_pointer_cast_initializer(void) {
      // Update the cast routine:
      fast_pointer_cast_blind<T, U>::cast = &fast_pointer_cast<T, U>;
    }
    static const fast_pointer_cast_initializer sc_init;
  };

  // Trivial case specialization
  template<class T>
  struct fast_pointer_cast_initializer<T, T> {
    static const fast_pointer_cast_initializer sc_init;
  };

  template<class T, class U>
  const fast_pointer_cast_initializer<T, U> fast_pointer_cast_initializer<T, U>::sc_init;
}
