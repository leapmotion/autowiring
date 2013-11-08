#include "stdafx.h"
#include "ReentrantCounter.h"

size_t ReentrantCounter::numberOfTimesEntered = 0;
boost::chrono::nanoseconds ReentrantCounter::globalTimeElapsedSinceStart(0);  

ReentrantCounter::ReentrantCounter(PerformanceCounter& duration) :
  duration(duration),
  startTime(boost::chrono::high_resolution_clock::now())
{
  numberOfTimesEntered++;
  myRecordedTimeElapsedSinceStart = globalTimeElapsedSinceStart;
  timesEnteredSinceIStarted = numberOfTimesEntered;
  //std::cout<<numberOfTimesEntered <<"OK?"<< globalTimeElapsedSinceStart.count()<< std::endl;
 }

ReentrantCounter::~ReentrantCounter(void) 
  
{
  endTime= boost::chrono::high_resolution_clock::now();
  // Unconditionally increment the hit count on this field:
  duration.hitCount++;
  bool iAmALeaf = (timesEnteredSinceIStarted == numberOfTimesEntered) ?true:false;
  if (iAmALeaf) {
  //std::cout<<"I am a leaf"<<std::endl;
  duration.lingerTime = endTime - startTime;
  } else if (!iAmALeaf){
  boost::chrono::nanoseconds difference = globalTimeElapsedSinceStart - myRecordedTimeElapsedSinceStart;
  duration.lingerTime = endTime - startTime - difference;
  }
  globalTimeElapsedSinceStart +=duration.lingerTime;
  //std::cout<<"Here is what I believe my global time elapsed since start is " << globalTimeElapsedSinceStart.count() << std::endl;
}