// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {

template<typename T>
T* thread_specific_ptr<T>::get() const {
  return static_cast<T*>(TlsGetValue(m_key));
}

template<typename T>
void thread_specific_ptr<T>::set(T* value) {
  TlsSetValue(m_key, value);
}

template<typename T>
void thread_specific_ptr<T>::init() {
  m_key = TlsAlloc();
  set(nullptr);
}

template<typename T>
void thread_specific_ptr<T>::freeTLS() {
  TlsFree(m_key);
}

} //namespace autowiring
