#pragma once
#include "Autowiring/Object.h"
#include <boost/thread/mutex.hpp>
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP

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
  template<class T>
  class Enclosure:
    public Object
  {
  public:
    Enclosure(void) :
      held(nullptr)
    {}

    Enclosure(const T& held) :
      held(std::move(held))
    {}

    Enclosure(T&& held) :
      held(std::move(held))
    {}

    T held;
  };

  // The enclosed type map and corresponding lock:
  boost::mutex m_lock;
  std::unordered_map<std::type_index, Object*> m_mp;

  // Satisfaction counters:
  std::vector<size_t> m_satCounters;

public:
  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  template<class T>
  bool Get(T*& out) {
    auto q = m_mp.find(typeid(T));
    if(q == m_mp.end()) {
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
      fx(q->first, q->second);
  }

  /// <summary>
  /// Decorates this packet with a particular type
  /// </summary>
  /// <remarks>
  /// Unlike Publish, the Decorate method is unconditional and will install the passed
  /// value regardless of whether any subscribers exist.
  /// </remarks>
  template<class T>
  void Decorate(const T& t) {
    Decorate<T>() = t;
  }

  template<class T>
  void Decorate(T&& t) {
    std::swap(Decorate<T>(), t);
  }

  template<class T>
  T& Decorate(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto& ptr = static_cast<Enclosure<T>*&>(m_mp[typeid(T)]);
    if(!ptr)
      ptr = new Enclosure<T>;
    return ptr->held;
  }

  /// <summary>
  /// Attempts to decorate this packet, if a subscriber exists
  /// </summary>
  /// <remarks>
  /// Publication is a conditional event, taking place if and only if an actual
  /// subscriber for this type exists.  Subscription takes place by a call to the
  /// Subscribe method on this packet, or the Subscribe method on the
  /// AutoPacketFactory.
  ///
  /// Because this type requires that the passed value be completely constructed
  /// when publication takes place, callers should generally only use this overload
  /// when a particular field is always available.
  /// </remarks>
  template<class T>
  void Publish(const T& val) {
    boost::lock_guard<boost::mutex> lk;
    auto q = m_mp.find(typeid(T));
    if(q != m_mp.end())
      static_cast<Enclosure<T>*>(q->second)->held = val;
  }

  /// <summary>
  /// Optimized overload of Publish which provides a storage space for serialization
  /// </summary>
  /// <returns>
  /// A slot which should be filled with the data to be published, or nullptr if there
  /// are no subscribers.
  /// </returns>
  /// <remarks>
  /// This override is useful when the published data should not be generated except
  /// when there are subscribers.  It is provided for convenience and optimization.
  /// </remarks>
  template<class T>
  T* Publish(void) const {
    boost::lock_guard<boost::mutex> lk;
    auto q = m_mp.find(typeid(T));
    return
      q == m_mp.end() ?
      nullptr :
      &static_cast<Enclosure<T>*>(q->second)->held;
  }

  /// <returns>
  /// True if the indicated type has been requested for use by some consumer
  /// </returns>
  /// <remarks>
  /// This method is used to determine whether some consumer invoked Subscribe on
  /// this type, or the corresponding factory, at some point prior to the call.
  /// </remarks>
  template<class T>
  bool HasSubscribers(void) const {
    auto q = m_mp.find(typeid(T));
    return q != m_mp.end();
  }
};

