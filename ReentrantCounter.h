#pragma once
#include <boost/chrono/system_clocks.hpp>

struct PerformanceCounter {
  PerformanceCounter(void) :
    hitCount(0),
    lingerTime(0)
  {}

  size_t hitCount;
  boost::chrono::nanoseconds lingerTime;
  

  ~PerformanceCounter(){
 // std::cout<<"Performance Counter DYING"<<std::endl;
  }
};

/// <summary>
/// Creates a new reentrant counter instance which stores duration information in the specified key
/// </summary>
class ReentrantCounter {
public:
  ReentrantCounter(PerformanceCounter& duration);
  ~ReentrantCounter(void);
  static boost::chrono::nanoseconds globalTimeElapsedSinceStart;  
  static ReentrantCounter * lastKnownObject;
private:
  PerformanceCounter& duration;
  ReentrantCounter * objectIObservedOnStart;
  // Construction time:
  boost::chrono::high_resolution_clock::time_point startTime;
  boost::chrono::high_resolution_clock::time_point endTime;
  boost::chrono::nanoseconds myRecordedTimeElapsedSinceStart;
};
