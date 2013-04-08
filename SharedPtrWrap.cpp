#include "stdafx.h"
#include "SharedPtrWrap.h"
#include "Autowirer.h"

cpp11::shared_ptr<Object> SharedPtrWrapBase::AsObject() {
  return cpp11::shared_ptr<Object>();
}

template void AutowirerErase<Autowirer::t_mpType>(Autowirer* pAutowirer, Autowirer::t_mpType::iterator q);
template void AutowirerErase<Autowirer::t_mpName>(Autowirer* pAutowirer, Autowirer::t_mpName::iterator q);