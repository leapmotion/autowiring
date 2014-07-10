#include "stdafx.h"
#include "ReentrantCounter.h"

ReentrantCounter * ReentrantCounter::lastKnownObject = 0;
std::chrono::nanoseconds ReentrantCounter::globalTimeElapsedSinceStart(0);

ReentrantCounter::ReentrantCounter(PerformanceCounter& duration) :
  duration(duration),
  startTime(std::chrono::high_resolution_clock::now())
{
  objectIObservedOnStart = lastKnownObject;
  lastKnownObject = this;
  if(!objectIObservedOnStart)globalTimeElapsedSinceStart = (std::chrono::nanoseconds(0));
 }

ReentrantCounter::~ReentrantCounter(void)
{
  endTime= std::chrono::high_resolution_clock::now();
  // Unconditionally increment the hit count on this field:
  duration.hitCount++;
  if (lastKnownObject == this) {
  //std::cout<<"I am a leaf"<<std::endl;
  duration.lingerTime = endTime - startTime;
  } else{
  // std::cout<<"I am not a leaf"<<std::endl;
  std::chrono::nanoseconds difference = globalTimeElapsedSinceStart - myRecordedTimeElapsedSinceStart;
  duration.lingerTime = endTime - startTime - difference;
  }
  globalTimeElapsedSinceStart +=duration.lingerTime;
  if (!objectIObservedOnStart) lastKnownObject = 0;
}