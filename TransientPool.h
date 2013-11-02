#pragma once
#include "autowiring_error.h"
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
    static_assert(!std::is_base_of<EventReceiverProxyBase, T>::value, "A transient member cannot be an event sender");
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

  /// <summary>
  /// Evicts all members from this pool
  /// </summary>
  /// <remarks>
  /// The only effect of this method is that it will prevent instances that still exist from
  /// getting any deferred or fired events via this pool.  Because the pool only retains weak
  /// pointers to the instances it manages, clearing the pool is guaranteed not to directly
  /// result in the destruction of any entities.
  /// </remarks>
  void Clear(void) {
    boost::lock_guard<boost::mutex> lk(m_transientLock);
    m_transient.clear();
  }

  virtual bool Add(std::shared_ptr<TransientContextMember> pMember) override {
    // Trivial return check:
    if(this->m_stop)
      throw_rethrowable autowiring_error("Cannot add an element to a terminated transient pool");

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

  virtual void AttachProxyRoutine(std::function<void (EventReceiver&)>&& eventProxy) override {
    // Special handling for our event proxy types:
    DispatchQueue::operator+=([this, eventProxy] () {
      this->PoolInvoke(eventProxy);
    });
  }
};

