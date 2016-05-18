// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "thread_specific_ptr.h"

namespace autowiring {

void* thread_specific_ptr_base::get() const {
  return pthread_getspecific(m_key);
}

void thread_specific_ptr_base::set(void* value) {
  pthread_setspecific(m_key, value);
}

void thread_specific_ptr_base::init() {
  pthread_key_create(&m_key, m_cleanupFunction);
}
  
void thread_specific_ptr_base::freeTLS() {
  pthread_key_delete(m_key);
}

}
