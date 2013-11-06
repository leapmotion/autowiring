#pragma once
#include <boost/chrono/system_clocks.hpp>

struct PerformanceCounter {
  PerformanceCounter(void) :
    hitCount(0),
    lingerTime(0)
  {}

  size_t hitCount;
  boost::chrono::nanoseconds lingerTime;
};

/// <summary>
/// Creates a new reentrant counter instance which stores duration information in the specified key
/// </summary>
class ReentrantCounter {
public:
  ReentrantCounter(PerformanceCounter& duration);
  ~ReentrantCounter(void);

private:
  PerformanceCounter& duration;

  // Construction time:
  boost::chrono::high_resolution_clock::time_point startTime;
};