#pragma once

namespace leap {

template<typename T>
T* thread_specific_ptr<T>::get() const {
  return static_cast<T*>(pthread_getspecific(m_key));
}

template<typename T>
void thread_specific_ptr<T>::set(T* value) {
  pthread_setspecific(m_key, value);
}

template<typename T>
void thread_specific_ptr<T>::init() {
  typedef void (*t_unsafeCleanup)(void*);
  pthread_key_create(&m_key, (t_unsafeCleanup)m_cleanupFunction);
}

}// namespace leap
