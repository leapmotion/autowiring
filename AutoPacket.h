#pragma once
#include "Autowired.h"
#include "Object.h"
#include <boost/thread/mutex.hpp>
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP

class AutoPacketFactory;
class AutoPacketSubscriber;

/// <summary>
/// A decorator-style processing packet
/// </summary>
/// <remarks>
/// A processing packet may be decorated with additional types as desired by the user.
/// The pipeline packet is not a type of context; querying the packet for an element of
/// a particular type will look for an element of precisely that type, not an inherited
/// type or a related interface.
///
/// Consumers who wish to advertise a particular field under multiple types must do so
/// manually with the Advertise function.
/// </remarks>
class AutoPacket
{
public:
  AutoPacket(void);
  ~AutoPacket(void);

private:
  /// <summary>
  /// A type enclosure, used to provide support similar to boost::any
  /// </summary>
  /// <remarks>
  /// We use this instead of boost::any so we can have access to a rvalue-move ctor.
  /// </remarks>
  template<class T>
  class Enclosure:
    public Object
  {
  public:
    Enclosure(void) {}

    Enclosure(const T& held) :
      held(held)
    {}

    Enclosure(T&& held) :
      held(std::move(held))
    {}

    T held;
  };

  // The associated packet factory:
  AutoRequired<AutoPacketFactory> m_factory;

  // The set of decorations currently attached to this object, and the associated lock:
  boost::mutex m_lock;
  std::unordered_map<std::type_index, Object*> m_mp;

  // Status counters, copied directly from the degree vector in the packet factory:
  std::vector<size_t> m_satCounters;

  /// <summary>
  /// Updates subscriber statuses given that the specified type information has been satisfied
  /// </summary>
  /// <param name="info">The decoration which was just added to this packet</param>
  /// <remarks>
  /// This method results in a call to the AutoFilter method on any subscribers which are
  /// satisfied by this decoration.
  /// </remarks>
  void UpdateSatisfaction(const std::type_info& info);

public:
  /// <summary>
  /// Clears all decorations and copies over all satisfaction counters
  /// </summary>
  void Reset(void);

  /// <summary>
  /// Detects the desired type, or throws an exception if such a type cannot be found
  /// </summary>
  template<class T>
  const T& Get(void) const {
    auto q = m_mp.find(typeid(T));
    if(q == m_mp.end())
      throw_rethrowable("Attempted to obtain a value which was not decorated on this packet");
    return static_cast<Enclosure<T>*>(q->second)->held;
  }

  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  template<class T>
  bool Get(T*& out) {
    auto q = m_mp.find(typeid(T));
    if(q == m_mp.end() || !q->second) {
      out = nullptr;
      return false;
    }
    out = static_cast<Enclosure<T>*>(q->second)->held;
    return true;
  }

  /// <summary>
  /// Provides the passed enumerator function with a list of all interior types
  /// </summary>
  template<class Fx>
  void Enumerate(Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto q = m_mp.begin(); q != m_mp.end(); q++)
      if(q->second)
        fx(q->first, q->second);
  }

  /// <summary>
  /// Decorates this packet with a particular type
  /// </summary>
  /// <returns>A reference to the internally persisted object</returns>
  /// <remarks>
  /// Unlike Publish, the Decorate method is unconditional and will install the passed
  /// value regardless of whether any subscribers exist.
  /// </remarks>
  template<class T>
  T& Decorate(T&& t) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto*& pObj = m_mp[typeid(T)];
    if(!pObj) {
      pObj = new Enclosure<T>(std::move(t));
      UpdateSatisfaction(typeid(T));
    }
    return static_cast<Enclosure<T>*>(pObj)->held;
  }

  /// <returns>
  /// True if the indicated type has been requested for use by some consumer
  /// </returns>
  /// <remarks>
  /// This method is used to determine whether some consumer invoked Subscribe on
  /// this type, or the corresponding factory, at some point prior to the call.
  /// </remarks>
  template<class T>
  bool HasSubscribers(void) const {return HasSubscribers(typeid(T));}

  bool HasSubscribers(const std::type_info& ti) const;
};

