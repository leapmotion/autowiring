// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include FUNCTIONAL_HEADER

// platform specific headers
#ifdef _MSC_VER
#include <windows.h>
#define TLS_KEY_TYPE DWORD
#else
#include <pthread.h>
#define TLS_KEY_TYPE pthread_key_t
#endif

// Platform specific token for thread local storage

namespace autowiring {

// <summary>
// Holds a ptr in thread local storage. Same interface as autoboost::thread_specific_ptr
// </summary>
template<typename T>
class thread_specific_ptr {
public:
  typedef void (*t_cleanupFunction)(T *);

  thread_specific_ptr(void)
  {
    init();
  }

  thread_specific_ptr(t_cleanupFunction cleanup):
    m_cleanupFunction(cleanup)
  {
    init();
  }

  virtual ~thread_specific_ptr(){
    reset();
    freeTLS();
  }

  T* get() const;

  T* operator->() const {
    return get();
  }

  T& operator*() const {
    return *get();
  }

  T* release() {
    T* const temp = get();
    set(nullptr);
    return temp;
  }

  void reset(T* new_value=nullptr) {
    T* const current_value = get();

    if (current_value == new_value)
      return;

    set(new_value);
    if (current_value && m_cleanupFunction)
      m_cleanupFunction(current_value);
  }

private:
  // Set thread specific value. Used by public facing "release" and "reset"
  void set(T* value);

  // Initialize thread specific storage
  void init();

  // Cleanup thread specific ptr when destoyed
  void freeTLS();

  // Functions called the cleanup old value
  t_cleanupFunction m_cleanupFunction = [](T* p) { delete p; };

  // Key to thread local storage
  TLS_KEY_TYPE m_key;
};

} //namespace autowiring

// Platform specifc functions
#ifdef _MSC_VER
#include "thread_specific_ptr_win.h"
#else //Mac and linux
#include "thread_specific_ptr_unix.h"
#endif

// cleanup definitions
#undef TLS_KEY_TYPE
