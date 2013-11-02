// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "GlobalCoreContext.h"

AutoCurrentContext::AutoCurrentContext(void):
  std::shared_ptr<CoreContext>(CoreContext::CurrentContext())
{
}

AutoGlobalContext::AutoGlobalContext(void):
  std::shared_ptr<CoreContext>(GlobalCoreContext::Get())
{
}

AutoCreateContext::AutoCreateContext(void):
  std::shared_ptr<CoreContext>(CoreContext::CurrentContext()->Create())
{
}