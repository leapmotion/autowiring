#pragma once
#include "AnySharedPointer.h"
#include "AutoCheckout.h"
#include "auto_out.h"
#include "auto_pooled.h"
#include "Object.h"
#include "optional_ptr.h"
#include "SatCounter.h"
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include EXCEPTION_PTR_HEADER

class AutoPacketFactory;
class AutoPacketProfiler;
class AutoPacketSubscriber;

template<class T>
struct subscriber_traits;

// Stores subscriber 'pointers' for a for a decorator of type 'ti'
struct AdjacencyEntry {
  // Indexes into the subscriber satisfaction vector.  Each entry in this list represents a single
  // subscriber, and an offset in the m_subscribers vector.  The second element in the pair is the
  // optional flag.
  std::vector<std::pair<size_t, bool>> subscribers;
};

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
  AutoPacket(AutoPacketFactory& factory);
  ~AutoPacket(void);

private:
  /// <remarks>
  /// A disposition holder, used to maintain initialization state on the key
  /// </remarks>
  struct DecorationDisposition:
    public AnySharedPointer
  {
    DecorationDisposition(void) :
      satisfied(false),
      isCheckedOut(false)
    {}

    // Flag, used by the caller, to mark this enclosure as satisfied
    bool satisfied;

    // Flag, set if the internally held object is currently checked out
    bool isCheckedOut;
  };

  // The associated packet factory:
  AutoPacketFactory& m_factory;
  
  // Map used to associate a decorator type with the adjacency entries for that type.
  typedef std::unordered_map<std::type_index, AdjacencyEntry> t_decMap;
  t_decMap m_decorations;

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
  /// Performs a "satisfaction pulse", which will avoid notifying any deferred filters
  /// </summary>
  void PulseSatisfaction(const std::type_info& info);

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// <param name="ready">Ready flag, set to false if the decoration should be marked unsatisfiable</param>
  template<class T>
  void CompleteCheckout(bool ready) {
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto& entry = m_mp[typeid(T)];
      assert(entry.satisfied);

      if(!ready)
        // Memory must be released, the checkout was cancelled
        entry->reset();

      // Reset the checkout flag before releasing the lock:
      assert(entry.isCheckedOut);
      entry.isCheckedOut = false;
    }

    UpdateSatisfaction(typeid(T), ready);
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
    if(q != m_mp.end() && q->second.satisfied) {
      auto& disposition = q->second;
      if(disposition) {
        out = disposition->as<T>().get();
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
      if(q->second.satisfied)
        fx(q->first, q->second.pEnclosure);
  }

  /// <summary>
  /// Checks out the specified type, providing it to the caller to be filled in
  /// </summary>
  /// <param name="use_pool">If set, allocations will use an object pool instead of an
  /// <remarks>
  /// The caller must call Ready on the returned value before it falls out of scope in order
  /// to ensure that the checkout is eventually committed.  The checkout will be committed
  /// when it falls out of scope if so marked.
  /// </remarks>
  template<class T>
  AutoCheckout<T> Checkout(bool use_pool = false) {
    // This allows us to install correct entries for decorated input requests
    typedef typename subscriber_traits<T>::type type;

    DecorationDisposition* entry;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);

      entry = &m_mp[typeid(type)];
      if(entry->satisfied)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      entry->satisfied = true;
      entry->isCheckedOut = true;
    }

    if(HasSubscribers<T>()) {
      auto& enclosure = (*entry = std::make_shared<T>());
      return AutoCheckout<T>(
        *this,
        enclosure.get(),
        &AutoPacket::CompleteCheckout<T>
      );
    }

    // Ensure we mark this entry unsatisfiable so that cleanup behavior works as expected
    entry->Release();
    return AutoCheckout<T>(*this, nullptr, &AutoPacket::CompleteCheckout<T>);
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
      if(entry.satisfied)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as appropriate:
      entry.satisfied = true;
      entry.Release();
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
      if(pEntry->satisfied)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      pEntry->satisfied = true;
    }

    auto& retVal = pEntry->Initialize<T>();
    *retVal = std::forward<T>(t);
    UpdateSatisfaction(typeid(T), true);
    return *retVal;
  }

  /// <summary>
  /// Attaches a decoration which will only be valid for the duration of the call
  /// </summary>
  /// <remarks>
  /// The attached decoration is only valid for AutoFilters which are valid during
  /// this call.
  /// </remarks>
  template<class T>
  void DecorateImmediate(const T* pt) {
    // Perform standard decoration with a short initialization:
    DecorationDisposition* pEntry;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      pEntry = &m_mp[typeid(T)];
      if(pEntry->satisfied)
        throw std::runtime_error("Cannot perform immediate decoration with type T, the requested decoration already exists");
      pEntry->satisfied = true;
    }

    // Pulse satisfaction:
    auto& enclosure = pEntry->Initialize(const_cast<T*>(pt));
    PulseSatisfaction(typeid(T));

    // Mark this entry as unsatisfiable:
    enclosure.Release();

    // Now trigger a rescan to hit any deferred, unsatisfiable entries:
    UpdateSatisfaction(typeid(T), false);
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

// Default behavior:
template<class T>
struct AutoPacketAdaptorHelper {
  static_assert(
    std::is_const<typename std::remove_reference<T>::type>::value ||
    !std::is_reference<T>::value,
    "If a decoration is desired, it must either be a const reference, or by value"
  );

  T operator()(AutoPacket& packet) const {
    return packet.Get<typename std::decay<T>::type>();
  }
};

template<class T>
struct AutoPacketAdaptorHelper<optional_ptr<T>> {
  // Optional pointer overload, tries to satisfy but doesn't throw if there's a miss
  optional_ptr<T> operator()(AutoPacket& packet) const {
    const typename std::decay<T>::type* out;
    if(packet.Get(out))
      return out;
    return nullptr;
  }
};

template<class T, bool checkout>
struct AutoPacketAdaptorHelper<auto_out<T, checkout>> {
  auto_out<T, checkout> operator()(AutoPacket& packet) const {
    return auto_out<T, checkout>(packet.Checkout<T>());
  }
};

template<>
struct AutoPacketAdaptorHelper<AutoPacket&> {
  AutoPacket& operator()(AutoPacket& packet) const {
    return packet;
  }
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

  template<class T>
  T Cast(void) const {
    AutoPacketAdaptorHelper<T> helper;
    return helper(packet);
  }
};
