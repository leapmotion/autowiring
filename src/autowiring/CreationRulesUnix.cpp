// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CreationRules.h"
#include <cstdlib>

void* autowiring::aligned_malloc(size_t ncb, size_t align) {
  void* pRetVal;
  if(posix_memalign(&pRetVal, ncb, align))
    return nullptr;
  return pRetVal;
}

void autowiring::aligned_free(void* ptr) {
  free(ptr);
}
