#include "stdafx.h"
#include "GlobalCoreContext.h"

void FillWithDefaults(void (*contextInit)(void)) {
  GlobalCoreContext::FillWithDefaults(contextInit);
}