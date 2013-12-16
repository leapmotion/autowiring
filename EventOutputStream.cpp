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
  return !m_OutputStream.str().size();;
}

size_t EventOutputStreamBase::GetSize(void) const {
  return m_OutputStream.str().size();
}

const void* EventOutputStreamBase::GetData(void) const {
  return nullptr;
}

void EventOutputStreamBase::Reset(void) {
  m_OutputStream.str("");
  m_OutputStream.clear();
}