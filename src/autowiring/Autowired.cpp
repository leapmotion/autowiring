// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Autowired.h"

AutoCurrentContext::AutoCurrentContext(void):
  std::shared_ptr<CoreContext>(CoreContext::CurrentContext())
{}

AutoGlobalContext::AutoGlobalContext(void):
  std::shared_ptr<GlobalCoreContext>(GlobalCoreContext::Get())
{}
