// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CreationRules.h"
#include <cstdlib>

void* autowiring::aligned_malloc(size_t ncb, size_t align) {
#if _POSIX_C_SOURCE < 200112L && _XOPEN_SOURCE < 600 && !defined(__APPLE__)
  return memalign(align, ncb);
#else
  void* pRetVal;
  if (posix_memalign(&pRetVal, align, ncb))
    return nullptr;
  return pRetVal;
#endif
}

void autowiring::aligned_free(void* ptr) {
  free(ptr);
}
