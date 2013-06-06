// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _OUTSTANDING_COUNT_TRACKER_H
#define _OUTSTANDING_COUNT_TRACKER_H
#include SHARED_PTR_HEADER

class CoreContext;

class OutstandingCountTracker
{
public:
  OutstandingCountTracker(std::shared_ptr<CoreContext> context);
  ~OutstandingCountTracker(void);

private:
  std::shared_ptr<CoreContext> m_context;
};

#endif
