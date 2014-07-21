#include "stdafx.h"
#include "Autowired.h"
#include "EventOutputStream.h"
#include <memory>
#include <cstring>

EventOutputStreamBase::EventOutputStreamBase(void)
{
}

EventOutputStreamBase::~EventOutputStreamBase(void)
{
}

bool EventOutputStreamBase::IsEmpty(void) const {
  return !m_OutputStream.str().size();
}

size_t EventOutputStreamBase::GetSize(void) const {
  return m_OutputStream.str().size();
}

const void* EventOutputStreamBase::GetData(void) const {
  auto intermed = m_OutputStream.str();
  char * retvalue = new char [ m_OutputStream.str().size()];
  std::memcpy(retvalue, m_OutputStream.str().c_str(),  m_OutputStream.str().size());
  return retvalue;
}

void EventOutputStreamBase::Reset(void) {
  m_OutputStream.str("");
  m_OutputStream.clear();
}
