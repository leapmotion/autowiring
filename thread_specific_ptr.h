#pragma once
#include "C++11/cpp11.h"
#include FUNCTIONAL_HEADER

// <summary>
// Holds a ptr in thread local storage. Same interface as boost::thread_specific_ptr
// </summary>
template<typename T>
class thread_specific_ptr {
public:
  thread_specific_ptr():
    m_cleanupFunction([this]{delete get();}),
    m_ptr(nullptr)
  {
    m_ptr = new T;
  }
  
  thread_specific_ptr(std::function<void(T*)> cleanup):
    m_cleanupFunction(cleanup),
    m_ptr(nullptr)
  {
    m_ptr = new T;
  }
  
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
    T* temp = get();
    m_ptr = nullptr;
    return temp;
  }
  
  void reset(T* new_value=nullptr) {
    if (m_ptr != new_value && m_ptr != nullptr) {
      m_cleanupFunction();
      m_ptr = new_value;
    }
  }
  
private:
  std::function<void(T*)> m_cleanupFunction;
  T* m_ptr;
};
