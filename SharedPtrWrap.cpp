#include "stdafx.h"
#include "SharedPtrWrap.h"

SharedPtrWrapBase::~SharedPtrWrapBase(void) {
}

cpp11::shared_ptr<Object> SharedPtrWrapBase::AsObject() {
  return cpp11::shared_ptr<Object>();
}