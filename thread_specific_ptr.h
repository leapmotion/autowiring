#pragma once
#include "C++11/cpp11.h"
#include FUNCTIONAL_HEADER

#if _WIN32
#define THREAD_LOCAL __declspec( thread )
#else
#define THREAD_LOCAL __thread
#endif

namespace leap {

// <summary>
// Holds a ptr in thread local storage. Same interface as boost::thread_specific_ptr
// </summary>
template<typename T>
class thread_specific_ptr {
public:
  thread_specific_ptr():
    m_cleanupFunction([](T* p){ delete p; })
  {}
  
  thread_specific_ptr(std::function<void(T*)> cleanup):
    m_cleanupFunction(cleanup)
  {}
  
  virtual ~thread_specific_ptr(){
    reset();
  }
  
  T* get() const {
    return m_ptr;
  }
  
  T* operator->() const {
    return get();
  }
  
  T& operator*() const {
    return *get();
  }
  
  T* release() {
    T* const temp = get();
    m_ptr = nullptr;
    return temp;
  }
  
  void reset(T* new_value=nullptr) {
    T* const current_value = get();
    
    if (current_value != new_value) {
      m_cleanupFunction(current_value);
      m_ptr = new_value;
    }
  }
  
private:
  // Functions called the cleanup old value. Defaults to "delete m_ptr"
  std::function<void(T*)> m_cleanupFunction;
  
  // Store our pointer in thread-local-storage
  THREAD_LOCAL static T* m_ptr;
};

  // Default value
template <typename T>
THREAD_LOCAL T* thread_specific_ptr<T>::m_ptr = nullptr;
  
} //namespace leap

