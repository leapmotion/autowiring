// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "at_exit.h"
#include "AutoCheckout.h"
#include "DecorationDisposition.h"
#include "demangle.h"
#include "hash_tuple.h"
#include "is_any.h"
#include "is_shared_ptr.h"
#include "MicroAutoFilter.h"
#include "ObjectPool.h"
#include <list>
#include <sstream>
#include <typeinfo>
#include CHRONO_HEADER
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include EXCEPTION_PTR_HEADER

class AutoPacket;
class AutoPacketFactory;
class AutoPacketProfiler;
struct AutoFilterDescriptor;

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
  AutoPacket(AutoPacket&&) = delete;
  AutoPacket(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding);

public:
  ~AutoPacket();

  static ObjectPool<AutoPacket> CreateObjectPool(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding);

private:
  // Saturation counters, constructed when the packet is created and reset each time thereafter
  // IMPORTANT: Elements in m_satCounters MUST be stationary, since they will be referenced!
  std::list<SatCounter> m_satCounters;
  size_t m_subscriberNum;

  // A pointer back to the factory that created us. Used for recording lifetime statistics.
  std::shared_ptr<AutoPacketFactory> m_parentFactory;

  // Hold the time point at which this packet was last initalized.
  std::chrono::high_resolution_clock::time_point m_initTime;

  // The set of decorations currently attached to this object, and the associated lock:
  // Decorations are indexed first by type and second by pipe terminating type, if any.
  // NOTE: This is a disambiguation of function reference assignment, and avoids use of constexp.
  typedef std::unordered_map<std::type_index, DecorationDisposition> t_decorationMap;
  t_decorationMap m_decorations;
  mutable std::mutex m_lock;

  /// <summary>
  /// Adds all AutoFilter argument information for a recipient
  /// </summary>
  void AddSatCounter(SatCounter& satCounter);

  /// <summary>
  /// Removes all AutoFilter argument information for a recipient
  /// </summary>
  void RemoveSatCounter(SatCounter& satCounter);

  // Outstanding count local and remote holds:
  std::shared_ptr<Object> m_outstanding;
  const std::shared_ptr<Object>& m_outstandingRemote;

  /// <summary>
  /// Resets satisfaction counters and decoration status.
  /// </summary>
  /// <remarks>
  /// Is it expected that AutoPacketFactory will call methods in the following order:
  /// AutoPacket(); //Construction in ObjectPool
  /// Initialize(); //Issued from ObjectPool
  /// Decorate();
  /// ... //More Decorate calls
  /// Finalize(); //Returned to ObjectPool
  /// Initialize();
  /// ... //More Issue & Return cycles
  /// ~AutoPacket(); //Destruction in ObjectPool
  /// Reset() must be called before the body of Initialize() in order to begin in the
  /// correct state. It must also be called after the body of Finalize() in order to
  /// avoid holding shared_ptr references.
  /// Therefore Reset() is called at the conclusion of both AutoPacket() and Finalize().
  /// </remarks>
  void Reset(void);

  /// <summary>
  /// Decrements subscribers requiring AutoPacket argument then calls all initializing subscribers.
  /// </summary>
  /// <remarks>
  /// Initialize is called when a packet is issued by the AutoPacketFactory.
  /// It is not called when the Packet is created since that could result in
  /// spurious calls when no packet is issued.
  /// </remarks>
  void Initialize(void);

  /// <summary>
  /// Last chance call with unsatisfied optional arguments.
  /// </summary>
  /// <remarks>
  /// This is called when the packet is returned to the AutoPacketFactory.
  /// It is not called when the Packet is destroyed, since that could result in
  /// suprious calles when no packet is issued.
  /// </remarks>
  void Finalize(void);

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

  /// <summary>Un-templated & locked component of Has</summary>
  bool UnsafeHas(const std::type_info& data) const;

  /// <summary>Un-templated & locked component of Checkout</summary>
  void UnsafeCheckout(AnySharedPointer* ptr, const std::type_info& data);

  /// <summary>Un-templated & locked component of CompleteCheckout</summary>
  void UnsafeComplete(bool ready, const std::type_info& data, DecorationDisposition*& entry);

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// <param name="ready">Ready flag, set to false if the decoration should be marked unsatisfiable</param>
  void CompleteCheckout(bool ready, const std::type_info& data);

public:
  /// <returns>
  /// True if this packet posesses a decoration of the specified type
  /// </returns>
  /// <remarks>
  /// Although "AutoPacket &" and "const AutoPacket&" argument types will be
  /// satisfied, the AutoPacket does not "have" these types.
  /// </remarks>
  template<class T>
  bool Has(void) const {
    std::lock_guard<std::mutex> lk(m_lock);
    return UnsafeHas(typeid(T));
  }

  /// <summary>
  /// Detects the desired type, or throws an exception if such a type cannot be found
  /// </summary>
  template<class T>
  const T& Get(void) const {
    static_assert(!std::is_same<T, AnySharedPointer>::value, "Oops!");

    const T* retVal;
    if(!Get(retVal)) {
      std::stringstream ss;
      ss << "Attempted to obtain a type " << autowiring::demangle(retVal)
         << " which was not decorated on this packet";
      throw std::runtime_error(ss.str());
    }
    return *retVal;
  }

  /// <summary>
  /// Determines whether this pipeline packet contains an entry of the specified type
  /// </summary>
  /// <remarks>
  /// This method is also used by DecorateImmediate to extract pointers to data that is
  /// valid ONLY during recursive satisfaction calls.
  /// </remarks>
  template<class T>
  bool Get(const T*& out) const {
    std::lock_guard<std::mutex> lk(m_lock);

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
  /// Shared pointer specialization of const T*&, used to obtain the underlying shared pointer for some type T
  /// </summary>
  /// <remarks>
  /// This specialization cannot be used to obtain a decoration which has been attached to this packet via
  /// DecorateImmediate.
  /// </remarks>
  template<class T>
  bool Get(const std::shared_ptr<T>*& out) const {
    std::lock_guard<std::mutex> lk(m_lock);
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
  /// Shared pointer specialization, used to obtain the underlying shared pointer for some type T
  /// </summary>
  /// <remarks>
  /// This method can return an argument of DecorateImmediate as a shared_ptr<T> without a deleter.
  /// PROBLEM: This use case implies that holding shared_ptr references to decorations is NOT SAFE.
  /// </remarks>
  template<class T>
  bool Get(std::shared_ptr<const T>& out) const {
    std::lock_guard<std::mutex> lk(m_lock);
    auto deco = m_decorations.find(typeid(T));
    if(deco != m_decorations.end() &&
       deco->second.satisfied) {
      auto& disposition = deco->second;
      if(disposition.m_decoration) {
        out = disposition.m_decoration->as<T>();
        return true;
      } else if (disposition.m_pImmediate) {
        // Wrap immediate reference in shared_ptr without deleter
        out = std::shared_ptr<const T>(reinterpret_cast<const T*>(disposition.m_pImmediate), [](const T*){});
        return true;
      }
    }
    out.reset();
    return false;
  }

  template<class T>
  const std::shared_ptr<const T>& GetShared(void) const {
    const std::shared_ptr<const T>* retVal;
    Get(retVal);
    return *retVal;
  }

  /// <summary>
  /// De-templated placement method
  /// </summary>
  void Put(AnySharedPointer&& in);

  /// <summary>
  /// Transfers ownership of argument to AutoPacket
  /// </summary>
  /// <remarks>
  /// This method may throw an exception.  Ownership is unconditionally transferred to this class
  /// even in the event an exception is thrown, thus the passed pointer is guaranteed to be cleaned
  /// up properly in all cases.
  /// </remarks>
  template<class T>
  void Put(T* in) {
    Put(AnySharedPointer(std::shared_ptr<T>(in)));
  }

  /// <summary>
  /// Shares ownership of argument with AutoPacket
  /// </summary>
  /// <remarks>
  /// This can be used to:
  /// - place data on the AutoPack from an ObjectPool
  /// - move data from one AutoPacket to another without copying
  /// - alias the type of a decoration on AutoPacket
  /// </remarks>
  template<class T>
  void Put(std::shared_ptr<T> in) {
    Put(AnySharedPointer(std::move(in)));
  }

  /// <summary>Shares all broadcast data from this packet with the recipient packet</summary>
  /// <remarks>
  /// This method should ONLY be called during the final-call sequence.
  /// This method is expected to be used to bridge data to a sibling context.
  /// Therefore, only broadcast data will be shared, since pipes between sibling
  /// contexts cannot be defined.
  /// Furthermore, types that are unsatisfied in this context will not be marked as
  /// unsatisfied in the recipient - only present data will be provided.
  /// </remarks>
  void ForwardAll(std::shared_ptr<AutoPacket> recipient) const;

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
    const std::type_info& data = typeid(T);

    /// Injunction to prevent existential loops:
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot decorate a packet with another packet");

    if(!ptr)
      throw std::runtime_error("Cannot checkout with shared_ptr == nullptr");

    // This allows us to install correct entries for decorated input requests
    AnySharedPointer any_ptr(ptr);
    {
      std::lock_guard<std::mutex> guard(m_lock);
      UnsafeCheckout(&any_ptr, data);
    }
    return AutoCheckout<T>(
      *this,
      ptr,
      &AutoPacket::CompleteCheckout
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
      entry.m_type = &typeid(T); // Ensure correct type if instantiated here
      if(entry.satisfied ||
         entry.isCheckedOut)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as permanently checked-out
      entry.isCheckedOut = true;
    }

    // Now trigger a rescan:
    MarkUnsatisfiable(typeid(T));
  }

  /// <summary>
  /// Decorates this packet with a particular type
  /// </summary>
  /// <returns>A reference to the internally persisted object</returns>
  /// <remarks>
  /// The Decorate method is unconditional and will install the passed
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
  /// Each decoration is only valid for AutoFilters which are valid during this call.
  /// If multiple values are specified, all will be simultaneously made valid and
  /// then invalidated.
  /// </remarks>
  template<class T, class... Ts>
  void DecorateImmediate(const T& immed, const Ts&... immeds) {
    // None of the inputs may be shared pointers--if any of the inputs are shared pointers, they must be attached
    // to this packet via Decorate, or else dereferenced and used that way.
    static_assert(
      !is_any<is_shared_ptr<T>, is_shared_ptr<Ts>...>::value,
      "DecorateImmediate must not be used to attach a shared pointer, use Decorate on such a decoration instead"
    );
    
    // These are the things we're going to be working with while we perform immediate decoration:
    static const std::type_info* s_argTypes [] = {&typeid(T), &typeid(Ts)...};
    static const size_t s_arity = 1 + sizeof...(Ts);
    const void* pvImmeds [] = {&immed, &immeds...};
    DecorationDisposition* pTypeSubs[s_arity];

    // Perform standard decoration with a short initialization:
    {
      std::lock_guard<std::mutex> lk(m_lock);
      for(size_t i = 0; i < s_arity; i++) {
        pTypeSubs[i] = &m_decorations[*s_argTypes[i]];
        pTypeSubs[i]->m_type = s_argTypes[i]; // Ensure correct type if instantiated here
        if(pTypeSubs[i]->satisfied ||
           pTypeSubs[i]->isCheckedOut) {
          std::stringstream ss;
          ss << "Cannot perform immediate decoration with type " << autowiring::demangle(*s_argTypes[i])
             << ", the requested decoration already exists";
          throw std::runtime_error(ss.str());
        }

        // Mark the entry as appropriate:
        pTypeSubs[i]->isCheckedOut = true;
        pTypeSubs[i]->satisfied = true;
        pTypeSubs[i]->m_pImmediate = pvImmeds[i];
      }
    }

    // Pulse satisfaction:
    MakeAtExit([this, &pTypeSubs] {
      // Mark entries as unsatisfiable:
      // IMPORTANT: isCheckedOut = true prevents subsequent decorations of this type
      for(DecorationDisposition*  pEntry : pTypeSubs) {
        pEntry->m_pImmediate = nullptr;
        pEntry->satisfied = false;
      }

      // Now trigger a rescan to hit any deferred, unsatisfiable entries:
      for(const std::type_info* ti : s_argTypes)
        MarkUnsatisfiable(*ti);
    }),
    PulseSatisfaction(pTypeSubs, s_arity);
  }

  /// <summary>
  /// Adds a recipient for data associated only with this issuance of the packet.
  /// </summary>
  /// <remarks>
  /// Recipients added in this way cannot receive piped data, since they are anonymous.
  /// </remarks>
  void AddRecipient(const AutoFilterDescriptor& descriptor);

  /// <returns>A reference to the satisfaction counter for the specified type</returns>
  /// <remarks>
  /// If the type is not a subscriber GetSatisfaction().GetType() == nullptr will be true
  /// </remarks>
  SatCounter GetSatisfaction(const std::type_info& subscriber) const;

  /// <returns>All subscribers to the specified data</returns>
  std::list<SatCounter> GetSubscribers(const std::type_info& data) const;

  /// <returns>All decoration dispositions associated with the data type</returns>
  /// <remarks>
  /// This method is useful for determining whether flow conditions (broadcast, pipes
  /// immediate decorations) resulted in missed data.
  /// </remarks>
  std::list<DecorationDisposition> GetDispositions(const std::type_info& data) const;

  /// <returns>True if the indicated type has been requested for use by some consumer</returns>
  bool HasSubscribers(const std::type_info& data) const;
};
