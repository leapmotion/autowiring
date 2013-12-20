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
  auto intermed = m_OutputStream.str();
  auto ptr = intermed.c_str();
  char * retvalue = new char [ m_OutputStream.str().size()];
  memcpy(retvalue, m_OutputStream.str().c_str(),  m_OutputStream.str().size());
  return retvalue;
}

void EventOutputStreamBase::Reset(void) {
  m_OutputStream.str("");
  m_OutputStream.clear();
}