#pragma once
#include "AnySharedPointer.h"
#include "at_exit.h"
#include "AutoCheckout.h"
#include "DecorationDisposition.h"
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include EXCEPTION_PTR_HEADER

struct SatCounter;
class AutoPacketFactory;
class AutoPacketProfiler;
class AutoFilterDescriptor;

template<class T>
struct subscriber_traits;

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
  AutoPacket(const AutoPacket& rhs) = delete;
  AutoPacket(AutoPacketFactory& factory);
  ~AutoPacket(void);

private:
  // A back-link to the previously issued packet in the packet sequence.  May potentially be null,
  // if this is the first packet issued by the packet factory.
  std::shared_ptr<AutoPacket> m_prior;

  // Saturation counters, constructed when the packet is created and reset each time thereafter
  std::vector<SatCounter> m_satCounters;

  // The set of decorations currently attached to this object, and the associated lock:
  mutable boost::mutex m_lock;
  typedef std::unordered_map<std::type_index, DecorationDisposition> t_decorationMap;
  t_decorationMap m_decorations;

  void ResolveOptions(void);

  /// <summary>
  /// Marks the specified entry as being unsatisfiable
  /// </summary>
  void MarkUnsatisfiable(const std::type_info& info);

  /// <summary>
  /// Updates subscriber statuses given that the specified type information has been satisfied
  /// </summary>
  /// <param name="info">The decoration which was just added to this packet</param>
  /// <remarks>
  /// This method results in a call to the AutoFilter method on any subscribers which are
  /// satisfied by this decoration.
  /// </remarks>
  void UpdateSatisfaction(const std::type_info& info);

  /// <summary>
  /// Performs a "satisfaction pulse", which will avoid notifying any deferred filters
  /// </summary>
  /// <remarks>
  /// A satisfaction pulse will call any AutoFilter instances which are satisfied by the
  /// decoration of the passed decoration types.  Such filters will be called even if
  /// some optional inputs remain outstanding.
  /// </remarks>
  void PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos);

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// <param name="ready">Ready flag, set to false if the decoration should be marked unsatisfiable</param>
  template<class T>
  void CompleteCheckout(bool ready) {
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto& entry = m_decorations[typeid(T)];

      if(!ready)
        // Memory must be released, the checkout was cancelled
        entry.m_decoration->reset();

      // Reset the checkout flag before releasing the lock:
      assert(entry.isCheckedOut);
      entry.isCheckedOut = false;
      entry.satisfied = true;
    }

    if(ready)
      UpdateSatisfaction(typeid(T));
    else
      MarkUnsatisfiable(typeid(T));
  }

public:
  /// <summary>
  /// Resolves all options, clears all decorations and resets all satisfaction counters
  /// </summary>
  void Reset(void);

  /// <returns>
  /// True if this packet posesses a decoration of the specified type
  /// </returns>
  template<class T>
  bool Has(void) const {
    auto q = m_decorations.find(typeid(T));
    if(q == m_decorations.end())
      return false;
    return q->second.satisfied;
  }

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

    auto q = m_decorations.find(typeid(T));
    if(q != m_decorations.end() && q->second.satisfied) {
      auto& disposition = q->second;
      if(disposition.m_decoration) {
        out = disposition.m_decoration->as<T>().get();
        return true;
      }

      // Second-chance satisfaction with an immediate
      if(disposition.m_pImmediate) {
        out = (T*) disposition.m_pImmediate;
        return true;
      }
    }

    out = nullptr;
    return false;
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

    AnySharedPointer slot;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      auto dFind = m_decorations.find(typeid(type));
      if(dFind == m_decorations.end())
        // No parties interested in this entry, return here
        return AutoCheckout<T>(*this, nullptr, &AutoPacket::CompleteCheckout<T>);

      auto& entry = dFind->second;
      if(entry.satisfied)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      if(entry.isCheckedOut)
        throw std::runtime_error("Cannot check out this decoration, it's already checked out elsewhere");
      entry.isCheckedOut = true;
      entry.wasCheckedOut = true;
    }

    // Construct while the lock was released
    auto ptr = std::make_shared<T>();

    // Have to find the entry _again_ within the context of a lock and satisfy it here:
    (boost::lock_guard<boost::mutex>)m_lock,
    m_decorations[typeid(type)].m_decoration = ptr;

    return AutoCheckout<T>(
      *this,
      ptr,
      &AutoPacket::CompleteCheckout<T>
    );
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
      auto& entry = m_decorations[typeid(T)];
      if(entry.wasCheckedOut)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as appropriate:
      entry.satisfied = true;
      entry.wasCheckedOut = true;
    }

    // Now trigger a rescan:
    MarkUnsatisfiable(typeid(T));
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
    typedef typename std::remove_reference<T>::type Type;

    DecorationDisposition* pEntry;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      pEntry = &m_decorations[typeid(Type)];
      if(pEntry->wasCheckedOut)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");

      // Mark the entry as appropriate:
      pEntry->satisfied = true;
      pEntry->wasCheckedOut = true;
    }

    SharedPointerSlotT<Type>& retVal = pEntry->m_decoration->init<Type>();
    retVal = std::make_shared<Type>(std::forward<T>(t));
    UpdateSatisfaction(typeid(Type));
    return *retVal;
  }

  /// <summary>
  /// Subscribers respond to the decoration arguments immediately or never for this packet.
  /// Optional argument resolution is forced for any subscriber requiring at least one
  /// argument of this method
  /// </summary>
  /// <remarks>
  /// Unlike Decorate, the arguments of DecorateImmediate are not copied.
  /// Each decoration is only valid for AutoFilters which are valid during
  /// this call.
  /// If multiple values are specified, all will be simultaneously made valid and
  /// then invalidated.
  /// </remarks>
  template<class... Ts>
  void DecorateImmediate(const Ts&... immeds) {
    // These are the things we're going to be working with while we perform immediate decoration:
    static const std::type_info* sc_typeInfo [] = {&typeid(Ts)...};
    const void* pvImmeds[] = {&immeds...};
    DecorationDisposition* pTypeSubs[sizeof...(Ts)];

    // Perform standard decoration with a short initialization:
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      for(size_t i = 0; i < sizeof...(Ts); i++) {
        pTypeSubs[i] = &m_decorations[*sc_typeInfo[i]];
        if(pTypeSubs[i]->wasCheckedOut)
          throw std::runtime_error("Cannot perform immediate decoration with type T, the requested decoration already exists");

        // Mark the entry as appropriate:
        pTypeSubs[i]->satisfied = true;
        pTypeSubs[i]->wasCheckedOut = true;

        pTypeSubs[i]->m_pImmediate = &pvImmeds[i];
      }
    }

    // Pulse satisfaction:
    MakeAtExit([this, &pTypeSubs] {
      // Mark entries as unsatisfiable:
      for(auto pEntry : pTypeSubs) {
        pEntry->satisfied = false;
        pEntry->m_pImmediate = nullptr;
      }

      // Now trigger a rescan to hit any deferred, unsatisfiable entries:
      for(auto ti : sc_typeInfo)
        MarkUnsatisfiable(*ti);
    }),
    PulseSatisfaction(pTypeSubs, sizeof...(Ts));
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
