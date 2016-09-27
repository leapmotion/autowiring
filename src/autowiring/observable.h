// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "marshaller.h"
#include "signal.h"

namespace autowiring {

/// <summary>
/// An unsynchronzied wrapper type that implements the observable pattern
/// </summary>
/// <remarks>
///
/// </remarks>
template<typename T>
class observable {
public:
  observable(void) = default;

  observable(const T& val) :
    val(val)
  {}

  observable(T&& val) :
    val(std::move(val))
  {}

  observable(observable&& rhs) :
    val(std::move(rhs.val)),
    onChanged(std::move(rhs.onChanged)),
    onBeforeChanged(std::move(rhs.onBeforeChanged))
  {}
  observable(const observable& rhs) :
    val(rhs.val)
  {}

  autowiring::signal<void()> onChanged;
  autowiring::signal<void(const T& oldValue, const T& newValue)> onBeforeChanged;

private:
  T val;

public:
  operator const T&(void) const { return val; }
  const T& operator*(void) const { return val; }
  const T& get(void) const { return val; }

  /// <summary>
  /// Retrieves the underlying value, without any protection
  /// </summary>
  /// <remarks>
  /// Users should make use of this with caution.  Changes to the returned value will not cause
  /// signals to be asserted at the correct time by this type; users are responsible for doing
  /// this on their own.
  /// </remarks>
  T& get(void) { return val; }

  observable& operator=(const T& rhs) {
    onBeforeChanged(val, rhs);
    val = rhs;
    onChanged();
    return *this;
  }

  observable& operator=(const observable& rhs)
  {
    return *this = rhs.get();
  }

  observable& operator=(T&& rhs) {
    onBeforeChanged(val, rhs);
    val = std::move(rhs);
    onChanged();
    return *this;
  }
};


template<typename T>
struct marshaller<observable<T>> :
  marshaller_base
{
  typedef observable<T> type;

  // Marshaller for the interior type
  marshaller<T> interior;

  std::string marshal(const void* ptr) const override {
    return interior.marshal(&static_cast<const type*>(ptr)->get());
  }

  void unmarshal(void* ptr, const char* szValue) const override {
    T value;
    interior.unmarshal(&value, szValue);
    *static_cast<type*>(ptr) = std::move(value);
  }

  void copy(void* lhs, const void* rhs) const override {
    *static_cast<observable<T>*>(lhs) = *static_cast<const observable<T>*>(rhs);
  }
};

}
