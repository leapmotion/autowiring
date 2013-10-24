// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThread.h"

void CoreThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
#if __APPLE__
    pthread_setname_np(m_name);
#else
    pthread_setname_np(pthread_self(), m_name);
#endif
}
