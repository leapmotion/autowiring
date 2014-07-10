#pragma once
#include <chrono>

struct PerformanceCounter {
  PerformanceCounter(void) :
    hitCount(0),
    lingerTime(0)
  {}

  size_t hitCount;
  std::chrono::nanoseconds lingerTime;
  

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
  static std::chrono::nanoseconds globalTimeElapsedSinceStart;
  static ReentrantCounter * lastKnownObject;
private:
  PerformanceCounter& duration;
  ReentrantCounter * objectIObservedOnStart;
  // Construction time:
  std::chrono::high_resolution_clock::time_point startTime;
  std::chrono::high_resolution_clock::time_point endTime;
  std::chrono::nanoseconds myRecordedTimeElapsedSinceStart;
};
