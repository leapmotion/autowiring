#include "stdafx.h"
#include "DispatchQueue.h"

DispatchQueue::DispatchQueue(void):
  m_aborted(false)
{
}

DispatchQueue::~DispatchQueue(void) {
  if(!m_dispatchQueue.empty())
    throw std::runtime_error("Dispatch queue was not empty at destructor time");
}