#pragma once
#include "Autowired.h"
#include "Object.h"
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP

class AutoPacketFactory;
class AutoPacketProfiler;
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

  template<class T>
  class AutoCheckout {
  public:
    AutoCheckout(void) :
      m_parent(nullptr),
      m_val(nullptr)
    {}

    AutoCheckout(AutoPacket& parent, T* val) :
      m_parent(&parent),
      m_val(val)
    {}

    AutoCheckout(AutoCheckout&& rhs) :
      m_parent(rhs.m_parent),
      m_val(rhs.m_val)
    {
      rhs.m_parent = nullptr;
      rhs.m_val = nullptr;
    }

    ~AutoCheckout(void) {
      if(m_val)
        m_parent->UpdateSatisfaction(typeid(T));
    }

  private:
    AutoPacket* m_parent;
    T* m_val;

  public:
    T* operator->(void) const { return m_val; }
    
    operator bool(void) const { return !!m_val; }

    operator T&(void) const { return *m_val; }

    AutoCheckout& operator=(AutoCheckout&& rhs) {
      std::swap(m_parent, rhs.m_parent);
      std::swap(m_val, rhs.m_val);
      return *this;
    }
  };

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

  /// <summary>
  /// A single subscription counter entry
  /// </summary>
  struct SatCounter {
    size_t remaining;
    boost::any subscriber;
  };

  // The associated packet factory:
  AutoRequired<AutoPacketFactory> m_factory;

  // Profiler, if one exists:
  Autowired<AutoPacketProfiler> m_profiler;

  // The set of decorations currently attached to this object, and the associated lock:
  boost::mutex m_lock;
  std::unordered_map<std::type_index, Object*> m_mp;

  // Status counters, copied directly from the degree vector in the packet factory:
  std::vector<SatCounter> m_satCounters;

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
  /// Releases all exterior references to autowired members
  /// </summary>
  void Release(void);

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
      throw_rethrowable std::runtime_error("Attempted to obtain a value which was not decorated on this packet");
    return static_cast<Enclosure<T>*>(q->second)->held;
  }

  template<class T>
  typename std::enable_if<
    std::is_same<AutoPacket, T>::value, T
  >::type& Get(void) {
    return *this;
  }

  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  template<class T>
  bool Get(T*& out) const {
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot obtain an non-const AutoPacket from a const AutoPacket");

    auto q = m_mp.find(typeid(T));
    if(q == m_mp.end() || !q->second) {
      out = nullptr;
      return false;
    }
    out = &static_cast<Enclosure<T>*>(q->second)->held;
    return true;
  }

  bool Get(const AutoPacket*& out) const {
    out = this;
    return true;
  }

  bool Get(AutoPacket*& out) {
    out = this;
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
  /// Checks out the specified type, providing it to the caller to be filled in
  /// </summary>
  /// <remarks>
  template<class T>
  AutoCheckout<T> Checkout(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    auto*& pObj = m_mp[typeid(T)];
    if(pObj)
      throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");

    if(!HasSubscribers<T>())
      return AutoCheckout<T>(*this, nullptr);

    auto enclosure = new Enclosure<T>();
    pObj = enclosure;
    return AutoCheckout<T>(*this, &enclosure->held);
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
    Object* retVal;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto*& pObj = m_mp[typeid(T)];
      if(pObj)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      retVal = pObj = new Enclosure<T>(std::move(t));
    }

    UpdateSatisfaction(typeid(T));
    return static_cast<Enclosure<T>*>(retVal)->held;
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

