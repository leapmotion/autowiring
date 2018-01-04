// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "thread_specific_ptr.h"

static const HMODULE hKernel32 = LoadLibrary("kernel32.dll");
static decltype(&FlsAlloc) g_FlsAlloc = (decltype(&FlsAlloc))GetProcAddress(hKernel32, "FlsAlloc");
static decltype(&FlsGetValue) g_FlsGetValue = (decltype(&FlsGetValue))GetProcAddress(hKernel32, "FlsGetValue");
static decltype(&FlsSetValue) g_FlsSetValue = (decltype(&FlsSetValue))GetProcAddress(hKernel32, "FlsSetValue");
static decltype(&FlsFree) g_FlsFree = (decltype(&FlsFree))GetProcAddress(hKernel32, "FlsFree");

static const bool init = [] {
  if (!g_FlsAlloc)
    g_FlsAlloc = [](autowiring::t_cleanupFunction) { return TlsAlloc(); };
  if (!g_FlsGetValue)
    g_FlsGetValue = TlsGetValue;
  if (!g_FlsSetValue)
    g_FlsSetValue = TlsSetValue;
  if (!g_FlsFree)
    g_FlsFree = TlsFree;

  return true;
}();

namespace autowiring {

void* thread_specific_ptr_base::get() const {
  return FlsGetValue(m_key);
}

void thread_specific_ptr_base::set(void* value) {
  FlsSetValue(m_key, value);
}

void thread_specific_ptr_base::init(void) {
  m_key = FlsAlloc(m_cleanupFunction);
  set(nullptr);
}

void thread_specific_ptr_base::freeTLS(void) {
  FlsFree(m_key);
}

}
