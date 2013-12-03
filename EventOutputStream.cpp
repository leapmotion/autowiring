#include "stdafx.h"
#include "EventOutputStream.h"

EventOutputStream::EventOutputStream(void)
{
}

EventOutputStream::~EventOutputStream(void)
{
}

bool EventOutputStream::IsEmpty(void) const {
  return true;
}

size_t EventOutputStream::GetSize(void) const {
  return 0;
}

const void* EventOutputStream::GetData(void) const {
  return nullptr;
}

void EventOutputStream::Reset(void) {
}