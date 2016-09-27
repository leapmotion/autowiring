// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CreationRules.h"

void* autowiring::aligned_malloc(size_t ncb, size_t align) {
  return _aligned_malloc(ncb, align);
}

void autowiring::aligned_free(void* ptr) {
  return _aligned_free(ptr);
}
