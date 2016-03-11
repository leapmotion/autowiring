// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "marshaller.h"
#include "spin_lock.h"
#include <array>

namespace autowiring {
  /// <summary>
  /// A configuration value tracker class used with the configuration systems
  /// </summary>
  /// <remarks>
  /// This class uses a double-buffered value approach to avoid synchronization requirements
  /// under asynchronous update.  It assumes a single point of use and multiple points of
  /// update.  The clear_dirty call can be used to simaltaneously switch the active value
  /// buffer and also clear the dirty flag.
  /// </remarks>
  template<typename T>
  struct config {
    config(void) = default;
    config(const config&) = delete;
    config(config&& rhs):
      dirty(rhs.dirty),
      valueIndex(rhs.valueIndex)
    {
      values[0] = std::move(rhs.values[0]);
      values[1] = std::move(rhs.values[1]);
    }

    explicit config(const T& value) { values[0] = value; }
    explicit config(T&& value) { values[0] = std::move(value); }

    template<typename U>
    explicit config(U&& value) {
      values[0] = std::forward<U&&>(value);
    }

  private:
    // Lock, used to move the asyncrhonous value to the value field
    autowiring::spin_lock lock;

    // Tracks whether the backing value has been updated
    bool dirty = true;

    // The index of the currently active value
    size_t valueIndex = 0;

    // The active value, advertised by accessors, and the dirty value, accessed
    // by assigners.
    std::array<T, 2> values;

  public:
    bool is_dirty(void) const { return dirty; }
    const T& get(void) const { return values[valueIndex]; }

    /// <summary>
    /// Clears the dirty bit and returns its value prior to the clear
    /// </summary>
    /// <returns>True if the field was dirty, false otherwise</returns>
    /// <remarks>
    /// This method must be synchronized with all accessors.
    /// </remarks>
    bool clear_dirty(void) {
      if (!dirty)
        return false;

      std::lock_guard<autowiring::spin_lock> lk(lock);
      valueIndex = !valueIndex;
      dirty = false;
      return true;
    }

    operator const T&(void) const { return get(); }
    const T* operator->(void) const { return &get(); }
    const T& operator*(void) const { return get(); }
    bool operator==(const T& rhs) const { return get() == rhs; }
    bool operator!=(const T& rhs) const { return get() != rhs; }

    config& operator=(const config& value) {
      // Local copy to hold the spin lock a minimum amount of time
      return *this = value.get();
    }

    template<typename U>
    config& operator=(const U& rhs) {
      return *this = T{ rhs };
    }

    template<typename U>
    config& operator=(U&& rhs) {
      std::lock_guard<autowiring::spin_lock> lk(lock);
      this->values[!valueIndex] = std::forward<U&&>(rhs);
      dirty = true;
      return *this;
    }
  };

  template<typename T>
  bool operator==(const T& lhs, const config<T>& rhs) { return rhs == lhs; }

  template<typename T>
  struct marshaller<config<T>> :
    marshaller_base
  {
    typedef config<T> type;

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
  };
}
