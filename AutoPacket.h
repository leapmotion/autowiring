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
    Enclosure(void) {}

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
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto& ptr = static_cast<Enclosure<T>*&>(m_mp[typeid(T)]);
    if(!ptr)
      ptr = new Enclosure<T>;
  }

  template<class T>
  void Decorate(T&& t) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto*& pObj = m_mp[typeid(T)];
    if(!pObj)
      pObj = new Enclosure<T>;
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

