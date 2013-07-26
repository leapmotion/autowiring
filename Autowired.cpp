// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "GlobalCoreContext.h"

AutoCurrentContext::AutoCurrentContext(void) {
  *this = CoreContext::CurrentContext();
}

AutoGlobalContext::AutoGlobalContext(void):
  std::shared_ptr<CoreContext>(GlobalCoreContext::Get())
{
}

AutoCreateContext::AutoCreateContext(void) {
  *this = CoreContext::CurrentContext()->Create();
}