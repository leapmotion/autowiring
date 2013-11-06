#include "stdafx.h"
#include "ReentrantCounter.h"

ReentrantCounter::ReentrantCounter(PerformanceCounter& duration) :
  duration(duration),
  startTime(boost::chrono::high_resolution_clock::now())
{}

ReentrantCounter::~ReentrantCounter(void)
{
  // Unconditionally increment the hit count on this field:
  duration.hitCount++;

  // TODO:  Compute total linger time, using globals as needed, and then
  // store the result in the local duration reference.  The implementation
  // shown here is certainly wrong!
  duration.lingerTime = boost::chrono::high_resolution_clock::now() - startTime;
}