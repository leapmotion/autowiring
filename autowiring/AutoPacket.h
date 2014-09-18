// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "at_exit.h"
#include "DataFlow.h"
#include "AutoCheckout.h"
#include "DecorationDisposition.h"
#include "demangle.h"
#include "is_shared_ptr.h"
#include "ObjectPool.h"
#include "is_any.h"
#include "MicroAutoFilter.h"
#include "hash_tuple.h"
#include <list>
#include <sstream>
#include <typeinfo>
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include EXCEPTION_PTR_HEADER

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
  AutoPacket(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding);

public:
  ~AutoPacket();

  static ObjectPool<AutoPacket> CreateObjectPool(AutoPacketFactory& factory, const std::shared_ptr<Object>& outstanding);

private:
  // Lifecycle mutability flag
  enum Mutability {
    enable_all = 0,
    disable_update = 1, //Disables update while resolving optional arguments
    disable_decorate = 2 //Disables decorate while resolving final calls
  } m_lifecyle;

  // Saturation counters, constructed when the packet is created and reset each time thereafter
  // IMPORTANT: Elements in m_satCounters MUST be stationary, since they will be referenced!
  std::list<SatCounter> m_satCounters;
  size_t m_subscriberNum;

  // The set of decorations currently attached to this object, and the associated lock:
  // Decorations are indexed first by type and second by pipe terminating type, if any.
  // NOTE: This is a disambiguation of function reference assignment, and avoids use of constexp.
  static std::tuple<std::type_index, std::type_index> DSIndex(const std::type_index& data, const std::type_index& source) {
    return std::make_tuple(data, source);
  }
  typedef std::unordered_map<std::tuple<std::type_index, std::type_index>, DecorationDisposition> t_decorationMap;
  t_decorationMap m_decorations;
  mutable std::mutex m_lock;

  /// <summary>
  /// Retrieve data flow information for a decoration
  /// </summary>
  /// <remarks>
  /// Broadcast is always true for added or snooping recipients.
  /// Pipes are always absent for added or snooping recipients.
  /// </remarks>
  autowiring::DataFlow GetDataFlow(const DecorationDisposition& entry) const;

  /// <summary>
  /// Retrieve data flow information from source
  /// </summary>
  autowiring::DataFlow GetDataFlow(const std::type_info& data, const std::type_info& source);

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
  /// Adds a recipient for data associated only with this issuance of the packet.
  /// </summary>
  void InitializeRecipient(const AutoFilterDescriptor& descriptor);

  /// <summary>
  /// Marks the specified entry as being unsatisfiable
  /// </summary>
  void MarkUnsatisfiable(const std::type_info& info, const std::type_info& source = typeid(void));

  /// <summary>
  /// Updates subscriber statuses given that the specified type information has been satisfied
  /// </summary>
  /// <param name="info">The decoration which was just added to this packet</param>
  /// <remarks>
  /// This method results in a call to the AutoFilter method on any subscribers which are
  /// satisfied by this decoration.
  /// </remarks>
  void UpdateSatisfaction(const std::type_info& info, const std::type_info& source = typeid(void));

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
  bool UnsafeHas(const std::type_info& data, const std::type_info& source = typeid(void)) const;

  /// <summary>Un-templated & locked component of Checkout</summary>
  void UnsafeCheckout(AnySharedPointer* ptr, const std::type_info& data, const std::type_info& source);

  /// <summary>Un-templated & locked component of CompleteCheckout</summary>
  void UnsafeComplete(bool ready, const std::type_info& data, const std::type_info& source,
                      DecorationDisposition* &broadDeco, DecorationDisposition* &pipedDeco);

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// <param name="ready">Ready flag, set to false if the decoration should be marked unsatisfiable</param>
  template<class T>
  void CompleteCheckout(bool ready, const std::type_info& source = typeid(void)) {
    DecorationDisposition* broadDeco = nullptr;
    DecorationDisposition* pipedDeco = nullptr;

    {
      std::lock_guard<std::mutex> guard(m_lock);
      // This allows us to retrieve correct entries for decorated input requests
      const std::type_info& data = typeid(typename subscriber_traits<T>::type);
      UnsafeComplete(ready, data, source, broadDeco, pipedDeco);
    }

    if(ready) {
      // Satisfy the base declaration first and then the shared pointer:
      if (broadDeco) {
        UpdateSatisfaction(broadDeco->m_decoration->type(), typeid(void));
        UpdateSatisfaction(broadDeco->m_decoration->shared_type(), typeid(void));
      }
      if (pipedDeco) {
        // NOTE: Only publish with source if pipes are declared - this prevents
        // added or snooping filters from satisfying piped input declarations.
        UpdateSatisfaction(pipedDeco->m_decoration->type(), source);
        UpdateSatisfaction(pipedDeco->m_decoration->shared_type(), source);
      }
    } else {
      if (broadDeco)
        MarkUnsatisfiable(broadDeco->m_decoration->type(), typeid(void));
      if (pipedDeco)
        MarkUnsatisfiable(pipedDeco->m_decoration->type(), source);
    }
  }

public:
  /// <returns>
  /// True if this packet posesses a decoration of the specified type
  /// </returns>
  /// <remarks>
  /// Although "AutoPacket &" and "const AutoPacket&" argument types will be
  /// satisfied, the AutoPacket does not "have" these types.
  /// </remarks>
  template<class T>
  bool Has(const std::type_info& source = typeid(void)) const {
    std::lock_guard<std::mutex> lk(m_lock);
    return UnsafeHas(typeid(T), source);
  }

  /// <summary>
  /// Detects the desired type, or throws an exception if such a type cannot be found
  /// </summary>
  template<class T>
  const T& Get(const std::type_info& source = typeid(void)) const {
    const T* retVal;
    if(!Get(retVal, source)) {
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
  template<class T>
  bool Get(const T*& out, const std::type_info& source = typeid(void)) const {
    std::lock_guard<std::mutex> lk(m_lock);

    auto q = m_decorations.find(DSIndex(typeid(T), source));
    if(q != m_decorations.end() &&
       q->second.satisfied) {
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
  bool Get(const std::shared_ptr<T>*& out, const std::type_info& source = typeid(void)) const {
    std::lock_guard<std::mutex> lk(m_lock);
    auto q = m_decorations.find(DSIndex(typeid(T), source));
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
  template<class T>
  bool Get(std::shared_ptr<const T>& out, const std::type_info& source = typeid(void)) const {
    const std::shared_ptr<T>* ptr = nullptr;
    Get(ptr, source);
    if (ptr) {
      out = *ptr;
      return true;
    }

    out.reset();
    return false;
  }

  /// <returns>
  /// The number of sources for the specified type supplied to the specified target
  /// </returns>
  /// <param name="target">Default value counts all broadcast data</param>
  /// <remarks>
  /// This method should ONLY be called during the final-call sequence.
  /// Before that time, the number of instances of a type may vary.
  /// </remarks>
  template<class T>
  int HasAll(const std::type_info& target = typeid(void)) const {
    std::lock_guard<std::mutex> lk(m_lock);

    int all = 0;
    for (auto& deco : m_decorations) {
      if (std::get<0>(deco.first) == typeid(T) &&
          deco.second.satisfied) {
        autowiring::DataFlow flow = GetDataFlow(deco.second);
        if (flow.output &&
            ((flow.broadcast && target == typeid(void)) ||
              flow.halfpipes.find(target) != flow.halfpipes.end())) {
          ++all;
        }
      }
    }
    return all;
  }

  /// <returns>
  /// All decorations of the specified type supplied to the specified target, indexed by source
  /// </summary>
  /// </returns>
  /// <param name="target">Default value retrieves all broadcast data</param>
  /// <remarks>
  /// This method should ONLY be called during the final-call sequence.
  /// Before that time, the number of instances of a type may vary.
  /// </remarks>
  template<class T>
  std::unordered_map<std::type_index, std::shared_ptr<T>> GetAll(const std::type_info& target = typeid(void)) const {
    std::lock_guard<std::mutex> lk(m_lock);

    std::unordered_map<std::type_index, std::shared_ptr<T>> all;
    for (const auto& deco : m_decorations) {
      if (std::get<0>(deco.first) == std::type_index(typeid(T)) &&
          deco.second.satisfied) {
        autowiring::DataFlow flow = GetDataFlow(deco.second);
        if (flow.output &&
            ((flow.broadcast && target == typeid(void)) ||
              flow.halfpipes.find(target) != flow.halfpipes.end())) {
          all[std::get<1>(deco.first)] = deco.second.m_decoration->as<T>();
        }
      }
    }
    return all;
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
  AutoCheckout<T> Checkout(std::shared_ptr<T> ptr, const std::type_info& source = typeid(void)) {
    /// Injunction to prevent existential loops:
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot decorate a packet with another packet");

    if(!ptr)
      throw std::runtime_error("Cannot checkout with shared_ptr == nullptr");

    // This allows us to install correct entries for decorated input requests
    const std::type_info& data = typeid(typename subscriber_traits<T>::type);
    AnySharedPointer any_ptr(ptr);
    {
      std::lock_guard<std::mutex> guard(m_lock);
      UnsafeCheckout(&any_ptr, data, source);
    }
    return AutoCheckout<T>(
      *this,
      ptr,
      &AutoPacket::CompleteCheckout<T>,
      source
    );
  }

  template<class T>
  AutoCheckout<T> Checkout(const std::type_info& source = typeid(void)) {
    return Checkout(std::make_shared<T>(), source);
  }

  /// <summary>
  /// Marks the named decoration as unsatisfiable
  /// </summary>
  /// <remarks>
  /// Marking a decoration as unsatisfiable immediately causes any filters with an optional
  /// input on this type to be called, if the remainder of their inputs are available.
  /// </remarks>
  template<class T>
  void Unsatisfiable(const std::type_info& source = typeid(void)) {
    {
      // Insert a null entry at this location:
      std::lock_guard<std::mutex> lk(m_lock);
      auto& entry = m_decorations[DSIndex(typeid(T), source)];
      entry.m_type = &typeid(T); // Ensure correct type if instantiated here
      if(entry.satisfied ||
         entry.isCheckedOut)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as permanently checked-out
      entry.isCheckedOut = true;
    }

    // Now trigger a rescan:
    MarkUnsatisfiable(typeid(T), source);
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
  const T& Decorate(T t, const std::type_info& source = typeid(void)) {
    return Decorate(std::make_shared<T>(std::forward<T>(t)), source);
  }

  /// <summary>
  /// Decoration method specialized for shared pointer types
  /// </summary>
  /// <remarks>
  /// This decoration method has the additional benefit that it will make direct use of the passed
  /// shared pointer.
  /// </remarks>
  template<class T>
  const T& Decorate(std::shared_ptr<T> t, const std::type_info& source = typeid(void)) {
    Checkout<T>(t, source).Ready();
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
    // TODO: DecorateImmediate can only broadcast - change this to allow sourced immediate decoration.
    const std::type_info& source = typeid(void);

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
        pTypeSubs[i] = &m_decorations[DSIndex(*s_argTypes[i], source)];
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
    MakeAtExit([this, &pTypeSubs, &source] {
      // Mark entries as unsatisfiable:
      // IMPORTANT: isCheckedOut = true prevents subsequent decorations of this type
      // IMPORTANT: m_pImmediate != nullptr records having used DecorateImmediate
      for(DecorationDisposition*  pEntry : pTypeSubs)
        pEntry->satisfied = false;

      // Now trigger a rescan to hit any deferred, unsatisfiable entries:
      for(const std::type_info* ti : s_argTypes)
        MarkUnsatisfiable(*ti, source);
    }),
    PulseSatisfaction(pTypeSubs, s_arity);
  }

  /// <summary>
  /// Adds a function to be called as an AutoFilter for this packet only.
  /// </summary>
  /// <remarks>
  /// Recipients added in this way cannot receive piped data, since they are anonymous.
  /// </remarks>
  template<class Ret, class... Args>
  void AddRecipient(std::function<Ret(Args...)> f) {
    InitializeRecipient(
      MakeAutoFilterDescriptor(std::make_shared<MicroAutoFilter<Ret, Args...>>(f))
    );
  }

  /// <returns>A reference to the satisfaction counter for the specified type</returns>
  /// <remarks>
  /// If the type is not a subscriber GetSatisfaction().GetType() == nullptr will be true
  /// </remarks>
  SatCounter GetSatisfaction(const std::type_info& subscriber) const;

  /// <returns>All subscribers to the specified data and source</returns>
  std::list<SatCounter> GetSubscribers(const std::type_info& data, const std::type_info& source = typeid(void)) const;

  /// <returns>All decoration dispositions associated with the data type</returns>
  /// <remarks>
  /// This method is useful for determining whether flow conditions (broadcast, pipes
  /// immediate decorations) resulted in missed data.
  /// </remarks>
  std::list<DecorationDisposition> GetDispositions(const std::type_info& data) const;

  /// <returns>True if the indicated type has been requested for use by some consumer</returns>
  bool HasSubscribers(const std::type_info& data, const std::type_info& source = typeid(void)) const;
};
