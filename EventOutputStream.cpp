//#include "Autowired.h"
#include "stdafx.h"
#include "EventOutputStream.h"

EventOutputStreamBase::EventOutputStreamBase(void)
{
}

EventOutputStreamBase::~EventOutputStreamBase(void)
{
}

bool EventOutputStreamBase::IsEmpty(void) const {
  return true;
}

size_t EventOutputStreamBase::GetSize(void) const {
  return 0;
}

const void* EventOutputStreamBase::GetData(void) const {
  return nullptr;
}

void EventOutputStreamBase::Reset(void) {
}