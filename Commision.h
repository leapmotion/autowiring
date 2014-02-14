#pragma once
#include <boost/thread/condition_variable.hpp>

class DispatchThunkBase;

class Commision {
public:
  // Normal Constructor
  Commision(DispatchThunkBase* p_thunk, boost::condition_variable& p_cond):
    m_fn(p_thunk),
    m_queueUpdated(p_cond)
  {}
  
  // Move Constructor
  Commision(Commision&& rhs):
    m_fn(rhs.m_fn),
    m_queueUpdated(rhs.m_queueUpdated)
  {
    rhs.m_fn = nullptr;
  }
  
  // Null Constructor
  Commision(boost::condition_variable& p_cond):
    m_fn(nullptr),
    m_queueUpdated(p_cond)
  {}
  
  ~Commision() {
    Commit();
  }
  
  // Commit the corresponding DispatchQueue element signalling that it is ready to run.
  // This is idempotent.
  void Commit() {
    if (m_fn) { //if we can perform a valid commit
      m_fn->Commit();
      m_queueUpdated.notify_all();
      m_fn = nullptr;
    }
  }
  
private:
  DispatchThunkBase* m_fn; //Shouldn't commit if nullptr
  boost::condition_variable& m_queueUpdated;
  
  // Copy constructor (don't use)
  Commision(const Commision& rhs);
};
