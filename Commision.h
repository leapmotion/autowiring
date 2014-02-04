#pragma once
#include <boost/thread/condition_variable.hpp>

class DispatchThunkBase;

class Commision {
public:
  // Normal Constructor
  Commision(DispatchThunkBase* p_thunk, boost::condition_variable* p_cond):
    fn(p_thunk),
    queueUpdated(p_cond),
    shouldCommit(true)
  {}
  
  // Copy constructor
  Commision(const Commision& rhs):
    fn(rhs.fn),
    queueUpdated(rhs.queueUpdated),
    shouldCommit(false)
  {}
  
  // Move Constructor
  Commision(Commision&& rhs):
    fn(rhs.fn),
    queueUpdated(rhs.queueUpdated),
    shouldCommit(true)
  {
    rhs.shouldCommit = false;
  }
  
  // Null Constructor
  Commision():
    shouldCommit(false)
  {}
  
  ~Commision() {
    Commit();
  }
  
  // Commit the corresponding DispatchQueue element signalling that it is ready to run
  // This is idempotent
  void Commit() {
    if (shouldCommit){
      fn->Commit();
      queueUpdated->notify_all();
      shouldCommit = false;
    }
  }
  
private:
  DispatchThunkBase* fn;
  boost::condition_variable* queueUpdated;
  bool shouldCommit;
};
