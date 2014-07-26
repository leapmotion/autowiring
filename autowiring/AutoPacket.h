// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "at_exit.h"
#include "AutoCheckout.h"
#include "DecorationDisposition.h"
#include "is_shared_ptr.h"
#include "ObjectPool.h"
#include "is_any.h"
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include EXCEPTION_PTR_HEADER

struct SatCounter;
class AutoPacketFactory;
class AutoPacketProfiler;
struct AutoFilterDescriptor;

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
private:
  AutoPacket(const AutoPacket& rhs) = delete;
  AutoPacket(AutoPacketFactory& factory);

public:
  ~AutoPacket(void);

  static ObjectPool<AutoPacket> CreateObjectPool(AutoPacketFactory& factory);

private:
  // A back-link to the previously issued packet in the packet sequence.  May potentially be null,
  // if this is the first packet issued by the packet factory.
  std::shared_ptr<AutoPacket> m_prior;

  // Saturation counters, constructed when the packet is created and reset each time thereafter
  std::vector<SatCounter> m_satCounters;

  // The set of decorations currently attached to this object, and the associated lock:
  mutable std::mutex m_lock;
  typedef std::unordered_map<std::type_index, DecorationDisposition> t_decorationMap;
  t_decorationMap m_decorations;

  /// <summary>
  /// Last change call with unsatisfied optional arguments
  /// </summary>
  void ResolveOptions(void);

  /// <summary>
  /// Resets counters, then decrements subscribers requiring AutoPacket argument.
  /// </summary>
  void Initialize(void);

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
      std::lock_guard<std::mutex> lk(m_lock);
      auto& entry = m_decorations[typeid(T)];

      if(!ready)
        // Memory must be released, the checkout was cancelled
        entry.m_decoration->reset();

      // Reset the checkout flag before releasing the lock:
      assert(entry.isCheckedOut);
      entry.isCheckedOut = false;
      entry.satisfied = true;
    }

    if(ready) {
      // Satisfy the base declaration first and then the shared pointer:
      UpdateSatisfaction(typeid(T));
      UpdateSatisfaction(typeid(std::shared_ptr<T>));
    }
    else
      MarkUnsatisfiable(typeid(T));
  }

public:
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
      throw_rethrowable autowiring_error("Attempted to obtain a value which was not decorated on this packet");
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
  /// Shared pointer specialization, used to obtain the underlying shared pointer for some type T
  /// </summary>
  /// <remarks>
  /// This specialization cannot be used to obtain a decoration which has been attached to this packet via
  /// DecorateImmediate
  /// </summary>
  template<class T>
  bool Get(const std::shared_ptr<T>*& out) const {
    auto q = m_decorations.find(typeid(T));
    if(q != m_decorations.end() && q->second.satisfied) {
      auto& disposition = q->second;
      if(disposition.m_decoration) {
        out = &disposition.m_decoration->as<T>();
        return true;
      }
    }
    
    out = nullptr;
    return false;
  }

  /// <summary>
  /// Checks out the specified type, providing it to the caller to be filled in
  /// </summary>
  /// <param name="ptr">If set, the initial value that will be held by the checkout</param>
  /// <remarks>
  /// The caller must call Ready on the returned value before it falls out of scope in order
  /// to ensure that the checkout is eventually committed.  The checkout will be committed
  /// when it falls out of scope if so marked.
  /// </remarks>
  template<class T>
  AutoCheckout<T> Checkout(std::shared_ptr<T> ptr) {
    // This allows us to install correct entries for decorated input requests
    typedef typename subscriber_traits<T>::type type;

    if(!ptr)
      throw std::runtime_error("Cannot checkout with shared_ptr == nullptr");

    AnySharedPointer slot;
    {
      std::lock_guard<std::mutex> lk(m_lock);
      auto& entry = m_decorations[typeid(type)];
      if(entry.satisfied)
        throw std::runtime_error("Cannot decorate this packet with type T, the requested decoration already exists");
      if(entry.isCheckedOut)
        throw std::runtime_error("Cannot check out this decoration, it's already checked out elsewhere");
      entry.isCheckedOut = true;
      entry.wasCheckedOut = true;
    }

    // Have to find the entry _again_ within the context of a lock and satisfy it here:
    {
      std::lock_guard<std::mutex> lk(m_lock);
      m_decorations[typeid(type)].m_decoration = ptr;
    }

    return AutoCheckout<T>(
      *this,
      ptr,
      &AutoPacket::CompleteCheckout<T>
    );
  }

  template<class T>
  AutoCheckout<T> Checkout(void) {
    return Checkout(std::make_shared<T>());
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
      std::lock_guard<std::mutex> lk(m_lock);
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
  const T& Decorate(T t) {
    return Decorate(std::make_shared<T>(std::forward<T>(t)));
  }

  /// <summary>
  /// Decoration method specialized for shared pointer types
  /// </summary>
  /// <remarks>
  /// This decoration method has the additional benefit that it will make direct use of the passed
  /// shared pointer.
  /// </remarks>
  template<class T>
  const T& Decorate(std::shared_ptr<T> t) {
    Checkout<T>(t).Ready();
    return *t;
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

    // None of the inputs may be shared pointers--if any of the inputs are shared pointers, they must be attached
    // to this packet via Decorate, or else dereferenced and used that way.
    static_assert(
      !is_any<is_shared_ptr<Ts>...>::value,
      "DecorateImmediate must not be used to attach a shared pointer, use Decorate on such a decoration instead"
    );

    // Perform standard decoration with a short initialization:
    {
      std::lock_guard<std::mutex> lk(m_lock);
      for(size_t i = 0; i < sizeof...(Ts); i++) {
        pTypeSubs[i] = &m_decorations[*sc_typeInfo[i]];
        if(pTypeSubs[i]->wasCheckedOut)
          throw std::runtime_error("Cannot perform immediate decoration with type T, the requested decoration already exists");

        // Mark the entry as appropriate:
        pTypeSubs[i]->satisfied = true;
        pTypeSubs[i]->wasCheckedOut = true;

        pTypeSubs[i]->m_pImmediate = pvImmeds[i];
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
      static const std::type_info* lamda_typeInfos [] = {&typeid(Ts)...};
      for(auto ti : lamda_typeInfos)
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
