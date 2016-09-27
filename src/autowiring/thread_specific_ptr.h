// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include FUNCTIONAL_HEADER

// platform specific headers
#ifdef _MSC_VER
#include <windows.h>
namespace autowiring {
  typedef void(__stdcall *t_cleanupFunction)(void*);
  typedef DWORD TLS_KEY_TYPE;
}
#else
#include <pthread.h>
namespace autowiring {
  typedef void(*t_cleanupFunction)(void*);
  typedef pthread_key_t TLS_KEY_TYPE;
}
#endif

namespace autowiring {

class thread_specific_ptr_base {
protected:
  thread_specific_ptr_base(t_cleanupFunction cleanupFunction) :
    m_cleanupFunction(cleanupFunction)
  {}

  ~thread_specific_ptr_base(void) {
    freeTLS();
  }

  // Key to thread local storage
  TLS_KEY_TYPE m_key;

  // Cleanup routine for the entry in the key
  const t_cleanupFunction m_cleanupFunction;

  // Gets teh assigned value
  void* get(void) const;

  // Set thread specific value. Used by public facing "release" and "reset"
  void set(void* value);

  // Initialize thread specific storage
  void init();

  // Cleanup thread specific ptr when destoyed
  void freeTLS();
};

/// <summary>
/// Holds a ptr in thread local storage. Same interface as autoboost::thread_specific_ptr
/// </summary>
template<typename T>
class thread_specific_ptr final :
  thread_specific_ptr_base
{
public:
  thread_specific_ptr(void) :
    thread_specific_ptr([](void* ptr) { delete static_cast<T*>(ptr); })
  {}

  thread_specific_ptr(t_cleanupFunction cleanupFunction) :
    thread_specific_ptr_base(cleanupFunction)
  {
    init();
  }

  ~thread_specific_ptr(void) {
    reset();
  }

public:
  T* get(void) const { return static_cast<T*>(thread_specific_ptr_base::get()); }

  T* release(void) {
    T* const temp = get();
    set(nullptr);
    return temp;
  }

  void reset(void) {
    m_cleanupFunction(get());
    set(nullptr);
  }

  void reset(T* new_value) {
    m_cleanupFunction(get());
    set(new_value);
  }

  // Operator overloads:
  const T& operator=(const T& new_value) {
    if (T* value = get()) {
      *value = new_value;
      return *value;
    } else {
      T* rv = new T{ new_value };
      set(rv);
      return *rv;
    }
  }

  void operator=(T&& new_value) {
    if (T* value = get())
      *value = std::move(new_value);
    else
      set(new T{ std::move(new_value) });
  }

  T* operator->() const { return get(); }
  T& operator*() const { return *get(); }
};

}
