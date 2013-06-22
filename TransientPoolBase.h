#pragma once
#include FUNCTIONAL_HEADER

class EventReceiver;
class TransientContextMember;

class TransientPoolBase:
  public virtual EventReceiver
{
public:
  virtual ~TransientPoolBase(void) {}

  /// <summary>
  /// Retrieves a witness type which may be dynamically cast to each supported recipient event type
  /// </summary>
  virtual const EventReceiver& GetWitness(void) const = 0;
  
  /// <summary>
  /// Adds the specified transient context member to the pool
  /// </sumamry>
  /// <remarks>
  /// Only a weak pointer is held internally to the passed member.  The member will stop
  /// receiving events as soon as it goes out of scope.
  ///
  /// This method may be called even if the passed member does not match the type of the
  /// derived pool.  In this case, the function will return false.
  /// </remarks>
  virtual bool Add(std::shared_ptr<TransientContextMember> pMember) = 0;

  /// <summary>
  /// Immediate fire routine
  /// </summary>
  virtual void PoolInvoke(const std::function<void (EventReceiver&)>& eventProxy) = 0;
};