// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <memory>
#include <stdexcept>

/// <summary>
/// Implements the "optional" concept
/// </summary>
/// <remarks>
/// TODO:  Replace this with a typedef to std::optional once the STL provides it
/// </remarks>
namespace autowiring {
template<typename T>
struct optional {
public:
  optional(void) = default;
  optional(const optional& rhs) = default;
  optional(optional&& rhs) :
    m_valid(rhs.m_valid)
  {
    if (rhs) {
      new(val) T{ std::move(rhs._value()) };
      rhs._value().~T();
      rhs.m_valid = false;
    }
  }
  optional(T&& value):
    m_valid(true)
  {
    new(val) T{ std::move(value) };
  }

  ~optional(void) {
    if (m_valid)
      reinterpret_cast<T*>(val)->~T();
  }

private:
  bool m_valid = false;
  uint8_t val[sizeof(T)];

  T& _value(void) { return *reinterpret_cast<T*>(val); }
  const T& _value(void) const { return *reinterpret_cast<const T*>(val); }

public:
  operator bool(void) const { return m_valid; }
  T& value(void) {
    if (!m_valid)
      throw std::runtime_error("Attempted to access an uninitialized optional value");
    return *reinterpret_cast<T*>(val);
  }
  const T& value(void) const { return const_cast<optional*>(this)->value(); }

  /// <summary>
  /// Passes a pointer to the internal object space to the passed callback
  /// </remarks>
  template<typename Fn>
  void placement(const Fn& placement) {
    placement(reinterpret_cast<T*>(val));
    m_valid = true;
  }

  // Indirection operators:
  T* operator->(void) { return &value(); }
  const T* operator->(void) const { return &value(); }
  T& operator*(void) { return value(); }
  const T& operator*(void) const { return value(); }

  // Comparisons
  bool operator==(const optional& rhs) const {
    return
      (!m_valid && !rhs.m_valid) ||
      (m_valid && rhs.m_valid && _value() == rhs._value());
  }
  bool operator==(const T& rhs) const { return m_valid && _value() == rhs; }
  bool operator!=(const optional& rhs) const { return !(*this == rhs); }
  bool operator!=(const T& rhs) const { return !(*this == rhs); }

  bool operator<(const optional& rhs) {
    return
      (!m_valid && !rhs.m_valid) ||
      (m_valid && rhs.m_valid && _value() < rhs._value());
  }
  bool operator<(const T& rhs) { return m_valid && _value() < rhs; }

  // Standard assignment
  optional& operator=(optional&& rhs) {
    if (rhs.m_valid)
      *this = std::move(rhs._value());
    else if (m_valid) {
      // RHS is invalid, we need to invalidate ourselves
      m_valid = false;
      _value().~T();
    }
    return *this;
  }
  optional& operator=(const optional& rhs) {
    if (rhs.m_valid)
      *this = rhs._value();
    else if(m_valid) {
      m_valid = false;
      _value().~T();
    }
    return *this;
  }

  template<typename U>
  typename std::enable_if<
    !std::is_same<typename std::decay<U>::type, optional>::value,
    optional
  >::type& operator=(U&& rhs) {
    if (m_valid)
      _value() = std::forward<U&&>(rhs);
    else {
      new(val) T{ std::forward<U&&>(rhs) };
      m_valid = true;
    }
    return *this;
  }

  void swap(optional& rhs) {
    if (m_valid)
      if (rhs.m_valid)
        std::swap(_value(), rhs._value());
      else {
        new(rhs.val) T{ std::move(_value()) };
        rhs.m_valid = true;
        m_valid = false;
        _value().~T();
      }
    else if (rhs.m_valid) {
      new(val) T{ std::move(rhs._value()) };
      m_valid = true;
      rhs.m_valid = false;
      rhs._value().~T();
    }
  }

  template<typename... Args>
  void emplace(Args&&... args) {
    if (m_valid)
      _value().~T();
    else
      m_valid = true;
    new(val) T{ args... };
  }
};
}

namespace std {
  template<typename T>
  void swap(::autowiring::optional<T>& lhs, ::autowiring::optional<T>& rhs) {
    lhs.swap(rhs);
  }
}
