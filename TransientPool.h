#pragma once
#include "CoreThread.h"
#include "EventReceiver.h"
#include "TransientPoolBase.h"
#include "TransientContextMember.h"

/// <summary>
/// Implements a transient pool with forwarding for member events
/// </summary>
/// <param name="T">The transient type to be relayed</param>
/// <param name="Witness">A polymorphic, constructable type which describes all of the events to be received</param>
/// <remarks>
/// The transient pool must not _itself_ be transient; rather, it is a durable collection of transient members.
/// </remarks>
template<class T, class Witness>
class TransientPool:
  public CoreThread,
  public TransientPoolBase
{
public:
  TransientPool(void) {
    static_assert(std::is_base_of<EventReceiver, Witness>::value, "Cannot operate a transient pool on a witness type that does not receive events");
    static_assert(std::is_base_of<TransientContextMember, T>::value, "Cannot operate a transient pool on a nontransient type");

    // Immediately ready
    Ready();
  }

private:
  // Witness type:
  const Witness m_witness;

  // Conditional lock, if the transient collection must be groomed or amended
  boost::mutex m_transientLock;

  // Internal collection of maintained transient instances:
  std::list<std::weak_ptr<T>> m_transient;

public:
  // Accessor methods:
  const EventReceiver& GetWitness(void) const override {return m_witness;}

  virtual bool Add(std::shared_ptr<TransientContextMember> pMember) override {
    auto casted = std::dynamic_pointer_cast<T, TransientContextMember>(pMember);
    if(!casted)
      return false;

    boost::lock_guard<boost::mutex> lk(m_transientLock);
    m_transient.push_back(casted);
    return true;
  }

  virtual void PoolInvoke(const std::function<void (EventReceiver&)>& eventProxy) override {
    boost::unique_lock<boost::mutex> lk(m_transientLock);
    for(auto q = m_transient.begin(); q != m_transient.end();) {
      auto ptr = q->lock();
      if(ptr) {
        // Invert the lock while we make the callout:
        lk.unlock();
        eventProxy(*ptr);
        lk.lock();

        // Element could not possibly have been deleted (it wasn't expired).  Pointer must
        // therefore remain stable, we can increment to the next entry while we hold a lock.
        q++;
      } else
        // Destroy this entity while we hold the lock
        q = m_transient.erase(q);
    }
  }

  virtual void operator+=(const std::function<void (EventReceiver&)>& eventProxy) override {
    // Special handling for our event proxy types:
    DispatchQueue::operator+=([this, eventProxy] () {
      this->PoolInvoke(eventProxy);
    });
  }
};

