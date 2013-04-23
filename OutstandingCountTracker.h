#ifndef _OUTSTANDING_COUNT_TRACKER_H
#define _OUTSTANDING_COUNT_TRACKER_H

class CoreContext;

class OutstandingCountTracker
{
public:
  OutstandingCountTracker(cpp11::shared_ptr<CoreContext> context);
  ~OutstandingCountTracker(void);

private:
  cpp11::shared_ptr<CoreContext> m_context;
};

#endif
