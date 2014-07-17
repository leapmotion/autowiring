#pragma once

namespace leap {

template<typename T>
T* thread_specific_ptr<T>::get() const {
  return static_cast<T*>(TlsGetValue(m_key));
}

template<typename T>
void thread_specific_ptr<T>::set(T* value) {
  TlsSetValue(key, value);
}

template<typename T>
void thread_specific_ptr<T>::init() {
  m_key = TlsAlloc();
}

template<typename T>
void thread_specific_ptr<T>::freeTLS() {
  TlsFree(m_key);
}

} //namespace leap
