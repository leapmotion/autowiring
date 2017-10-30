// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  namespace detail {
    // Holds true if type T can be copied safely
    template<typename T>
    struct can_copy {
      static const bool value =
        !std::is_abstract<T>::value &&
        std::is_copy_constructible<T>::value;
    };

    // Utility type for handling dereferencing of an input value
    template<typename T, typename = void>
    struct dereferencer
    {
      static_assert(std::is_copy_constructible<T>::value, "T must be copy constructable");
      static_assert(!std::is_abstract<T>::value, "T cannot be abstract");

      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(T&& val) : val(std::move(val)) {}
      T val;
      const T& operator*(void) const { return val; }
    };

    template<typename T>
    struct dereferencer<T&&, void>
    {
      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(T&& val) : val(std::move(val)) {}
      T val;
      const T& operator*(void) const { return val; }
    };

    /// <summary>
    /// Copyable reference argument
    /// </summary>
    template<typename T>
    struct dereferencer<T&, typename std::enable_if<!can_copy<T>::value>::type> {
      dereferencer(const dereferencer& rhs) : val(rhs.val) {}
      dereferencer(const T& val) : val(val) {}
      const T& val;
      const T& operator*(void) const { return val; }
    };

    /// <summary>
    /// Non-copyable reference argument
    /// </summary>
    template<typename T>
    struct dereferencer<T&, typename std::enable_if<can_copy<T>::value>::type> {
      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(const T& val) : val(val) {}
      T val;
      const T& operator*(void) const { return val; }
    };
  }
}
