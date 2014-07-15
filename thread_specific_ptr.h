#pragma once
#include "C++11/cpp11.h"
#include FUNCTIONAL_HEADER

#include <pthread.h>

// Platform specific token for thread local storage

namespace leap {

// <summary>
// Holds a ptr in thread local storage. Same interface as boost::thread_specific_ptr
// </summary>
template<typename T>
class thread_specific_ptr {
public:
  thread_specific_ptr():
    m_cleanupFunction([](void* p){ delete static_cast<T*>(p); })
  {
    pthread_key_create(&m_key, m_cleanupFunction);
  }
  
  thread_specific_ptr(std::function<void(T*)> cleanup):
    m_cleanupFunction([](void* p){ delete static_cast<T*>(p); })
  {
    pthread_key_create(&m_key, m_cleanupFunction);
  }
  
  virtual ~thread_specific_ptr(){
    reset();
    pthread_key_delete(m_key);
  }
  
  T* get() const {
    return static_cast<T*>(pthread_getspecific(m_key));
  }
  
  T* operator->() const {
    return get();
  }
  
  T& operator*() const {
    return *get();
  }
  
  T* release() {
    T* const temp = get();
    pthread_setspecific(m_key, nullptr);
    return temp;
  }
  
  void reset(T* new_value=nullptr) {
    T* const current_value = get();
    if (current_value != new_value) {
      pthread_setspecific(m_key, new_value);
    }
  }
  
private:
  // Functions called the cleanup old value. Defaults to "delete m_ptr"
  void (*m_cleanupFunction)(void *);
  
  pthread_key_t m_key;
};
  
} //namespace leap

