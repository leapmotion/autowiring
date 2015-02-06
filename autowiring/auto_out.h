// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

/// <summary>
/// Declares an output of an AutoFilter method via an argument.
/// </summary>
/// <remarks>
/// Lazy allocation is implemented, and is triggered on destruction.
/// Any attempt to access the referenced data will trigger
/// instantiation and a commitment to output the data.
/// Moved construction and assignment removes the option to output
/// from the r-value operand, but does *not* force an output.
/// </remarks>
template<class T>
class auto_out
{
public:
  typedef T id_type;

  auto_out(const auto_out<T>& rhs):
    m_value(rhs.m_value)
  {}

  auto_out(std::shared_ptr<T>& value) :
    m_value(value)
  {}

protected:
  std::shared_ptr<T>& m_value;

public:
  /// <summary>
  /// Returns the dumb pointer to the underlying output value
  /// </summary>
  T* get(void) const { return m_value.get(); }

  /// <summary>
  /// Releases the internal shared pointer, preventing it from being decorated on the packet
  /// </summary>
  void reset(void) {
    m_value.reset();
  }

  // Convenience operator overloads
  T& operator* () { return *m_value; }
  T* operator-> () { return m_value.get(); }
  explicit operator bool() const { return m_value; }
  operator T&(void) { return *m_value; }
  operator std::shared_ptr<T>&(void) { return m_value; }

  /// <summary>Output will be shared data provided by rhs</summary>
  /// <remarks>
  /// This method faciliates output of shared ObjectPool data, with
  /// destructors defined accordingly.
  /// </remarks>
  auto_out& operator=(std::shared_ptr<T> rhs) {
    std::swap(m_value, rhs);
    return *this;
  }

  auto_out& operator=(T&& rhs) {
    m_value = std::make_shared<T>(std::forward<T&&>(rhs));
    return *this;
  }

  auto_out& operator=(const T& rhs) {
    m_value = std::make_shared<T>(rhs);
    return *this;
  }
};
