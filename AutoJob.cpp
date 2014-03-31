#include "stdafx.h"
#include "AutoJob.h"

AutoJob::AutoJob(const char* name) :
  ContextMember(name)
{}

bool AutoJob::DelayUntilCanAccept(void) {
  return false;
}

bool AutoJob::CanAccept(void) const {
  return false;
}