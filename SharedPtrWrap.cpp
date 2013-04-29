#include "stdafx.h"
#include "SharedPtrWrap.h"
#include "Autowirer.h"

std::shared_ptr<Object> SharedPtrWrapBase::AsObject() {
  return std::shared_ptr<Object>();
}
