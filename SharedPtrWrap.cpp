#include "stdafx.h"
#include "SharedPtrWrap.h"
#include "Autowirer.h"

cpp11::shared_ptr<Object> SharedPtrWrapBase::AsObject() {
  return cpp11::shared_ptr<Object>();
}