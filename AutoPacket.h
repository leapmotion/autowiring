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
      if(m_val)
        m_parent->CompleteCheckout<T>(m_ready);
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
  /// A short enclosure, useful when we don't want to generate needless constructor calls
  /// </summary>
  template<class T>
  class EnclosureShort:
    public Object
  {
  public:
    // A redirection-style constructor, useful when the internally held value is not going to
    // be used
    EnclosureShort(T* ptr = nullptr) :
      ptr(ptr)
    {}

    // The pointer to the space referred to by this enclosure.  Generally, this type is an
    // Enclosure, and this will point to the "held" member.  Potentially, however, this field
    // could be null, indicating that the decoration is unsatisfiable, or the field could
    // point somewhere else entirely, indicating that the decoration is an immediate (IE, will
    // only be valid during a call to DecorateImmediate).
    T* ptr;
  };

  /// <summary>
  /// A type enclosure, used to provide support similar to boost::any
  /// </summary>
  /// <remarks>
  /// We use this instead of boost::any so we can have access to a rvalue-move ctor.
  /// </remarks>
  template<class T>
  class Enclosure:
    public EnclosureShort<T>
  {
  public:
    Enclosure(void) :
      EnclosureShort(&m_held)
    {}

    Enclosure(const T& held) :
      EnclosureShort(&m_held),
      m_held(held)
    {}

    Enclosure(T&& held) :
      EnclosureShort(&m_held),
      m_held(std::move(held))
    {}

  private:
    T m_held;

  public:
    operator T*(void) { return ptr; }
    operator T&(void) { return *ptr; }
    operator const T&(void) const { return *ptr; }
  };
  
  /// <remarks>
  /// A disposition holder, used to maintain initialization state on the key
  /// </remarks>
  struct DecorationDisposition {
    DecorationDisposition(void) :
      initialized(false),
      pEnclosure(nullptr)
    {}

    ~DecorationDisposition(void) {
      Reset();
      delete (void*) pEnclosure;
    }

    // Flag, used by the caller, to mark the enclosure as "taken".  This value is
    // generally managed externally.
    bool initialized;

    // The enclosure proper:
    Object* pEnclosure;

    template<class T>
    Enclosure<T>& Initialize(void) {
      if(pEnclosure)
        new (pEnclosure) Enclosure<T>();
      else
        pEnclosure = new Enclosure<T>();
      return static_cast<Enclosure<T>&>(*pEnclosure);
    }

    template<class T>
    Enclosure<T>& Initialize(T&& val) {
      if(pEnclosure)
        new (pEnclosure) Enclosure<T>(std::move(val));
      else
        pEnclosure = new Enclosure<T>(std::move(val));
      return static_cast<Enclosure<T>&>(*pEnclosure);
    }

    void Reset(void) {
      if(initialized) {
        initialized = false;
        pEnclosure->~Object();
      }
    }

    template<class T>
    void Unsatisfiable(void) {
      if(!pEnclosure)
        pEnclosure = reinterpret_cast<Enclosure<T>*>(new char[sizeof(Enclosure<T>)]);
      *new (pEnclosure) EnclosureShort<T>();
    }

    template<class T>
    const T* Obtain(void) const {
      return initialized ? static_cast<Enclosure<T>&>(*pEnclosure).ptr : nullptr;
    }
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
  mutable boost::mutex m_lock;
  mutable std::unordered_map<std::type_index, DecorationDisposition> m_mp;

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
  template<class T>
  void RevertSatisfaction(void) {
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto& entry = m_mp[typeid(T)];
      entry.initialized = true;
      entry.Unsatisfiable<T>();
    }

    // Now we update the satisfaction:
    UpdateSatisfaction(typeid(T), false);
  }

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// <param name="ready">Ready flag, set to false if the decoration should be marked unsatisfiable</param>
  template<class T>
  void CompleteCheckout(bool ready) {
    if(ready)
      UpdateSatisfaction(typeid(T), true);
    else
      RevertSatisfaction<T>();
  }

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
    const T* retVal;
    if(!Get(retVal))
      throw_rethrowable std::runtime_error("Attempted to obtain a value which was not decorated on this packet");
    return *retVal;
  }

  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  template<class T>
  bool Get(const T*& out) const {
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot decorate a packet with another packet");

    auto q = m_mp.find(typeid(T));
    if(q != m_mp.end() && q->second.initialized) {
      auto enclosure = static_cast<Enclosure<T>*>(q->second.pEnclosure);
      if(enclosure->ptr) {
        out = enclosure->ptr;
        return true;
      }
    }

    out = nullptr;
    return false;
  }

  /// <summary>
  /// Provides the passed enumerator function with a list of all interior types
  /// </summary>
  template<class Fx>
  void Enumerate(Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto q = m_mp.begin(); q != m_mp.end(); q++)
      if(q->second.initialized)
        fx(q->first, q->second.pEnclosure);
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
    DecorationDisposition* entry;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);

      entry = &m_mp[typeid(T)];
      if(entry->initialized)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      entry->initialized = true;
    }

    return
      HasSubscribers<T>() ?
      AutoCheckout<T>(*this, entry->Initialize<T>()) :
      AutoCheckout<T>(*this, nullptr);
  }

  /// <summary>
  /// Marks the named decoration as unsatisfiable
  /// </summary>
  /// <remarks>
  /// Marking a decoration as unsatisfiable immediately causes any filters with an optional
  /// input on this type to be called, if the remainder of their inputs are available.
  /// </remarks>
  template<class T>
  void Unsatisfiable(void) {
    {
      // Insert a null entry at this location:
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto& entry = m_mp[typeid(T)];
      if(entry.initialized)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as appropriate:
      entry.initialized = true;
      entry.Unsatisfiable<T>();
    }

    // Now trigger a rescan:
    UpdateSatisfaction(typeid(T), false);
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
    DecorationDisposition* pEntry;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      pEntry = &m_mp[typeid(T)];
      if(pEntry->initialized)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      pEntry->initialized = true;
    }

    auto& retVal = pEntry->Initialize(std::move(t));
    UpdateSatisfaction(typeid(T), true);
    return retVal;
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

/// <summary>
/// Utility extractive wrapper
/// </summary>
/// <remarks>
/// This class is useful in instances where implicitly casting is more convenient
/// than explicitly naming the desired type and pulling it out of the packet with Get.
///
/// So, for instance, rather than:
///
/// type t = packet.Get<type>();
///
/// One could do:
///
/// type t = AutoPacketAdaptor(packet);
///
/// The packet extractor also provides additional utility extraction routines, which
/// makes it a much more attractive option than trying to manually invoke Get on the
/// packet directly.
/// </remarks>
class AutoPacketAdaptor {
public:
  AutoPacketAdaptor(AutoPacket& packet):
    packet(packet)
  {}

private:
  AutoPacket& packet;

public:
  // Reflexive overloads:
  operator AutoPacket*(void) const {return &packet;}
  operator AutoPacket&(void) const {return packet;}
  operator std::shared_ptr<AutoPacket>(void) const { return packet.shared_from_this(); }

  // Optional pointer overload, tries to satisfy but doesn't throw if there's a miss
  template<class T>
  operator optional_ptr<T>(void) const {
    typename const std::decay<T>::type* out;
    if(packet.Get(out))
      return out;
    return nullptr;
  }

  // This is our last-ditch attempt:  Run a query on the underlying packet
  template<class T>
  operator const T&(void) const {
    return packet.Get<typename std::decay<T>::type>();
  }
};