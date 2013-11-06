#pragma once
#include "Autowired.h"
#include "Object.h"
#include "optional_ptr.h"
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
class AutoPacket:
  public std::enable_shared_from_this<AutoPacket>
{
public:
  AutoPacket(void);
  ~AutoPacket(void);

  template<class T>
  class AutoCheckout {
  public:
    AutoCheckout(void) :
      m_parent(nullptr),
      m_val(nullptr),
      m_ready(false)
    {}

    AutoCheckout(AutoPacket& parent, T* val) :
      m_parent(&parent),
      m_val(val),
      m_ready(false)
    {}

    AutoCheckout(AutoCheckout&& rhs) :
      m_parent(rhs.m_parent),
      m_val(rhs.m_val),
      m_ready(rhs.m_ready)
    {
      rhs.m_parent = nullptr;
      rhs.m_val = nullptr;
    }

    ~AutoCheckout(void) {
      if(m_val) {
        if(m_ready)
          m_parent->UpdateSatisfaction(typeid(T), true);
        else
          m_parent->RevertSatisfaction(typeid(T));
      }
    }

  private:
    AutoPacket* m_parent;
    T* m_val;
    bool m_ready;

  public:
    /// <summary>
    /// Causes the wrapped packet to be committed when the checkout is destroyed
    /// </summary>
    void Ready(void) {
      m_ready = true;
    }
    
    // Operator overloads:
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
    Enclosure(void) :
      optional(&held)
    {}

    Enclosure(const T& held) :
      held(held),
      optional(&held)
    {}

    Enclosure(T&& held) :
      held(std::move(held)),
      optional(&held)
    {}

    T held;

    // An optional pointer, used during optional satisfaction.  Without this, there would be
    // no durable location where a pointer to "held" could be found.
    optional_ptr<T> optional;
  };

  /// <summary>
  /// A single subscription counter entry
  /// </summary>
  struct SatCounter {
    // The MANDATORY remaining counter:
    size_t remaining;

    // The OPTIONAL remaining counter:
    size_t optional;

    boost::any subscriber;

    /// <summary>
    /// Decrements the optional, or mandatory counter based on the selection
    /// </summary>
    /// <returns>True if there are decorations yet unsatisfied, optional or not</returns>
    bool Decrement(bool is_optional) {
      is_optional ? optional-- : remaining--;
      return remaining || optional;
    }
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
  /// Interior routine to UpdateSatisfaction
  /// </summary>
  void UpdateSatisfactionSpecific(size_t subscriberIndex);

  /// <summary>
  /// Updates subscriber statuses given that the specified type information has been satisfied
  /// </summary>
  /// <param name="info">The decoration which was just added to this packet</param>
  /// <remarks>
  /// This method results in a call to the AutoFilter method on any subscribers which are
  /// satisfied by this decoration.
  /// </remarks>
  void UpdateSatisfaction(const std::type_info& info, bool is_satisfied);

  /// <summary>
  /// Reverses a satisfaction that was issued, as in by a checkout, but not completed
  /// </summary>
  void RevertSatisfaction(const std::type_info& info);

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
    T* retVal;
    if(!Get(retVal))
      throw_rethrowable std::runtime_error("Attempted to obtain a value which was not decorated on this packet");
    return *retVal;
  }

  template<class T>
  T& Get(void) {
    T* retVal;
    if(!Get(retVal))
      throw_rethrowable std::runtime_error("Attempted to obtain a value which was not decorated on this packet");
    return *retVal;
  }

  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  template<class T>
  bool Get(const T*& out) const {
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot obtain an non-const AutoPacket from a const AutoPacket");

    auto q = m_mp.find(typeid(T));
    if(q == m_mp.end()) {
      out = nullptr;
      return false;
    }

    out = &static_cast<Enclosure<T>*>(q->second)->held;
    return true;
  }

  template<class T>
  bool Get(const optional_ptr<T>*& out) const {
    static const optional_ptr<T> empty;

    auto q = m_mp.find(typeid(T));
    out =
      q != m_mp.end() ?
      &static_cast<Enclosure<T>&>(*q->second).optional :
      // Couldn't find this entry--whatever, it was optional anyway.  Point it at a
      // reference of our one-and-only statically null optional.
      &empty;
    return true;
  }

  bool Get(const AutoPacket*& out) const {
    out = this;
    return true;
  }


  template<class T>
  bool Get(T*& out) {
    return const_cast<const AutoPacket*>(this)->Get(const_cast<const T*&>(out));
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
  /// The caller must call Ready on the returned value before it falls out of scope in order
  /// to ensure that the checkout is eventually committed.  The checkout will be committed
  /// when it falls out of scope if so marked.
  /// </remarks>
  template<class T>
  AutoCheckout<T> Checkout(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(m_mp.count(typeid(T)))
      throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");

    auto*& pObj = m_mp[typeid(T)];
    if(!HasSubscribers<T>())
      return AutoCheckout<T>(*this, nullptr);

    auto enclosure = new Enclosure<T>();
    pObj = enclosure;
    return AutoCheckout<T>(*this, &enclosure->held);
  }

  /// <summary>
  /// Marks the named decoration as unsatisfiable
  /// </summary>
  /// <remarks>
  /// Marking a decoration as unsatisfiable 
  template<class T>
  void Unsatisfiable(void) {
    // Insert a null entry at this location:
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(m_mp.count(typeid(T)))
      throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");
    m_mp[typeid(T)] = nullptr;
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
    Object** ppObject;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      if(m_mp.count(typeid(T)))
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");

      ppObject = &m_mp[typeid(T)];
    }

    *ppObject = new Enclosure<T>(std::move(t));
    UpdateSatisfaction(typeid(T), true);
    return static_cast<Enclosure<T>*>(*ppObject)->held;
  }

  /// <sumamry>
  /// Attaches a decoration which will only be valid for the duration of the call
  /// </summary>
  /// <remarks>
  /// The attached decoration is only valid for AutoFilters which are valid during
  /// this call.
  /// </remarks>
  template<class T>
  void DecorateImmediate(T&& t) {
    // Satisfy:
    Decorate(std::move(&t));

    // Now we have to evict the rhs to prevent subsequent satisfactions from trying
    // to obtain this field:
  }

  /// <returns>
  /// True if the indicated type has been requested for use by some consumer
  /// </returns>
  /// <remarks>
  /// This method is used to determine whether an AutoFilter recipient existed
  /// for the specified type at the time the packet was created
  /// </remarks>
  template<class T>
  bool HasSubscribers(void) const {return HasSubscribers(typeid(T));}

  bool HasSubscribers(const std::type_info& ti) const;
};

