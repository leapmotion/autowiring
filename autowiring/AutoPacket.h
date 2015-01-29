// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "at_exit.h"
#include "auto_id.h"
#include "AutoFilterDescriptorInput.h"
#include "DecorationDisposition.h"
#include "demangle.h"
#include "is_any.h"
#include "is_shared_ptr.h"
#include "TeardownNotifier.h"
#include <typeinfo>
#include CHRONO_HEADER
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include MUTEX_HEADER

class AutoPacket;
class AutoPacketInternal;
class AutoPacketFactory;
class AutoPacketProfiler;
class CoreContext;
struct AutoFilterDescriptor;
struct AutoFilterDescriptorInput;

template<class MemFn>
struct Decompose;

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
  public std::enable_shared_from_this<AutoPacket>,
  public TeardownNotifier
{
public:
  AutoPacket(const AutoPacket& rhs) = delete;
  AutoPacket(AutoPacket&&) = delete;

  // Must hold the lock to 'factory' when calling this constructor
  AutoPacket(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding);
  ~AutoPacket();

protected:
  // A pointer back to the factory that created us. Used for recording lifetime statistics.
  const std::shared_ptr<AutoPacketFactory> m_parentFactory;

  // The successor to this packet
  std::shared_ptr<AutoPacketInternal> m_successor;

  // Hold the time point at which this packet was last initalized.
  std::chrono::high_resolution_clock::time_point m_initTime;

  // Outstanding count local and remote holds:
  const std::shared_ptr<void> m_outstanding;

  // Pointer to a forward linked list of saturation counters, constructed when the packet is created
  SatCounter* m_firstCounter;

  // The set of decorations currently attached to this object, and the associated lock:
  // Decorations are indexed first by type and second by pipe terminating type, if any.
  // NOTE: This is a disambiguation of function reference assignment, and avoids use of constexp.
  typedef std::unordered_map<DecorationKey, DecorationDisposition> t_decorationMap;
  t_decorationMap m_decorations;

  mutable std::mutex m_lock;

  /// <summary>
  /// Checks out the decoration named by the specified type information and attaches the specified immediate pointer to it
  /// </summary>
  /// <remarks>
  /// An immediate checkout differs from a standard checkout in that the internally held decoration is only temporarily
  /// available.  Thus, callers are either satisfied at the point of decoration, or will not be satisfied for that
  /// type.
  /// </remarks>
  DecorationDisposition& DecorateImmediateUnsafe(const DecorationKey& key, const void* pvImmed);

  /// <summary>
  /// Adds all AutoFilter argument information for a recipient
  /// </summary>
  void AddSatCounter(SatCounter& satCounter);

  /// <summary>
  /// Removes all AutoFilter argument information for a recipient
  /// </summary>
  void RemoveSatCounter(const SatCounter& satCounter);

  /// <summary>
  /// Marks the specified entry as being unsatisfiable
  /// </summary>
  void MarkUnsatisfiable(const DecorationKey& key);
  
  /// <summary>
  /// Marks timeshifted decorations on successor packets as unsatisfiable
  /// </summary>
  void MarkSuccessorsUnsatisfiable(DecorationKey type);

  /// <summary>
  /// Updates subscriber statuses given that the specified type information has been satisfied
  /// </summary>
  /// <param name="info">The decoration which was just added to this packet</param>
  /// <remarks>
  /// This method results in a call to the AutoFilter method on any subscribers which are
  /// satisfied by this decoration.
  /// </remarks>
  void UpdateSatisfaction(const DecorationKey& info);

  /// <summary>
  /// Performs a "satisfaction pulse", which will avoid notifying any deferred filters
  /// </summary>
  /// <remarks>
  /// A satisfaction pulse will call any AutoFilter instances which are satisfied by the
  /// decoration of the passed decoration types.  Such filters will be called even if
  /// some optional inputs remain outstanding.
  /// </remarks>
  void PulseSatisfaction(DecorationDisposition* pTypeSubs[], size_t nInfos);

  /// <summary>Unsynchronized runtime counterpart to Has</summary>
  bool HasUnsafe(const DecorationKey& key) const;

  /// <summary>
  /// Performs a decoration operation but does not attach priors to successors.
  /// </summary>
  void DecorateUnsafeNoPriors(const AnySharedPointer& ptr, const DecorationKey& key);

  /// <summary>Runtime counterpart to Decorate</summary>
  void Decorate(const AnySharedPointer& ptr, DecorationKey key);

  /// <summary>
  /// Invoked from a checkout when a checkout has completed
  /// </summary>
  void UnsafeComplete(const DecorationKey& data);

  /// <summary>
  /// The portion of Successor that must run under a lock
  /// </summary>
  std::shared_ptr<AutoPacket> SuccessorUnsafe(void);

  /// <summary>
  /// Retrieves the decoration disposition corresponding to some type
  /// </summary>
  /// <returns>The disposition, if the decoration exists and is satisfied, otherwise nullptr</returns>
  const DecorationDisposition* GetDisposition(const DecorationKey& ti) const;

  /// <returns>True if the indicated type has been requested for use by some consumer</returns>
  bool HasSubscribers(const DecorationKey& key) const;

  /// <returns>A reference to the satisfaction counter for the specified type</returns>
  /// <remarks>
  /// If the type is not a subscriber GetSatisfaction().GetType() == nullptr will be true
  /// </remarks>
  const SatCounter& GetSatisfaction(const std::type_info& subscriber) const;

  /// <summary>
  /// Throws a formatted runtime error corresponding to the case where an absent decoration was demanded
  /// </summary>
  static void ThrowNotDecoratedException(const DecorationKey& key);

public:
  /// <returns>
  /// The number of distinct decoration types on this packet
  /// </returns>
  size_t GetDecorationTypeCount(void) const;

  /// <returns>
  /// A copy of the decoration dispositions collection
  /// </returns>
  /// <remarks>
  /// This is a diagnostic method, users are recommended to avoid the use of this routine where possible
  /// </remarks>
  t_decorationMap GetDecorations(void) const;

  /// <returns>
  /// True if this packet posesses a decoration of the specified type
  /// </returns>
  /// <remarks>
  /// Although "AutoPacket &" and "const AutoPacket&" argument types will be
  /// satisfied, the AutoPacket does not "have" these types.
  /// </remarks>
  template<class T>
  bool Has(int tshift=0) const {
    std::lock_guard<std::mutex> lk(m_lock);
    return HasUnsafe(DecorationKey(auto_id<T>::key(), tshift));
  }

  /// <summary>
  /// Detects the desired type, or throws an exception if such a type cannot be found
  /// </summary>
  template<class T>
  const T& Get(int tshift=0) const {
    static_assert(!std::is_same<T, AnySharedPointer>::value, "Oops!");

    const T* retVal;
    if (!Get(retVal, tshift))
      ThrowNotDecoratedException(DecorationKey(auto_id<T>::key(), tshift));
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
  bool Get(const T*& out, int tshift=0) const {
    const DecorationDisposition* pDisposition = GetDisposition(DecorationKey(auto_id<T>::key(), tshift));
    if (pDisposition) {
      if (pDisposition->m_decorations.size() == 1) {
        out = static_cast<const T*>(pDisposition->m_decorations[0]->ptr());
        return true;
      }

      // Second-chance satisfaction with an immediate
      if (pDisposition->m_pImmediate) {
        out = (T*) pDisposition->m_pImmediate;
        return true;
      }
    }

    out = nullptr;
    return false;
  }

  template<class T>
  bool DEPRECATED(Get(const std::shared_ptr<T>*& out) const, "This version of Get is deprecated due to the dangers it implies, do not use");

  /// <summary>
  /// Shared pointer specialization of const T*&, used to obtain the underlying shared pointer for some type T
  /// </summary>
  /// <param name="tshift">The number back to retrieve</param>
  /// <remarks>
  /// This specialization cannot be used to obtain a decoration which has been attached to this packet via
  /// DecorateImmediate.
  /// </remarks>
  template<class T>
  bool Get(const std::shared_ptr<const T>*& out, int tshift=0) const {
    // Decoration must be present and the shared pointer itself must also be present
    const DecorationDisposition* pDisposition = GetDisposition(DecorationKey(auto_id<T>::key(), tshift));
    if (!pDisposition || pDisposition->m_decorations.size() != 1) {
      out = nullptr;
      return false;
    }

    out = &pDisposition->m_decorations[0]->as_unsafe<const T>();
    return true;
  }

  /// <summary>
  /// Shared pointer specialization, used to obtain the underlying shared pointer for some type T
  /// </summary>
  /// <remarks>
  /// This method can return an argument of DecorateImmediate as a shared_ptr<T> without a deleter.
  /// PROBLEM: This use case implies that holding shared_ptr references to decorations is NOT SAFE.
  /// </remarks>
  template<class T>
  bool Get(std::shared_ptr<const T>& out, int tshift = 0) const {
    std::lock_guard<std::mutex> lk(m_lock);
    auto deco = m_decorations.find(DecorationKey(auto_id<T>::key(), tshift));
    if(deco != m_decorations.end() && deco->second.m_state == DispositionState::Satisfied) {
      auto& disposition = deco->second;
      if(disposition.m_decorations.size() == 1) {
        out = disposition.m_decorations[0]->as_unsafe<T>();
        return true;
      }
    }
    out.reset();
    return false;
  }

  template<class T>
  const std::shared_ptr<const T>& GetShared(int tshift = 0) const {
    const std::shared_ptr<const T>* retVal;
    Get(retVal, tshift);
    return *retVal;
  }

  /// <summary>
  /// Returns a null-terminated temporary buffer containing all decorations
  /// </summary>
  /// <returns>The null-terminated temporary buffer</returns>
  /// <remarks>
  /// The returned buffer must be freed with std::return_temporary_buffer
  /// </remarks>
  template<class T>
  const T** GetAll(int tshift = 0) const {
    std::lock_guard<std::mutex> lk(m_lock);
    auto q = m_decorations.find(DecorationKey(auto_id<T>::key(), tshift));

    // If decoration doesn't exist, return empty null-terminated buffer
    if (q == m_decorations.end()) {
      const T** retVal = std::get_temporary_buffer<const T*>(1).first;
      retVal[0] = nullptr;
      return retVal;
    }

    const auto& decorations = q->second.m_decorations;
    const T** retVal = std::get_temporary_buffer<const T*>(decorations.size() + 1).first;
    for (size_t i = 0; i < decorations.size(); i++)
      retVal[i] = static_cast<const T*>(decorations[i]->ptr());
    retVal[decorations.size()] = nullptr;
    return retVal;
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
  /// Marks the named decoration as unsatisfiable
  /// </summary>
  /// <remarks>
  /// Marking a decoration as unsatisfiable immediately causes any filters with an optional
  /// input on this type to be called, if the remainder of their inputs are available.
  /// </remarks>
  template<class T>
  void Unsatisfiable(void) {
    DecorationKey key(auto_id<T>::key());
    {
      // Insert a null entry at this location:
      std::lock_guard<std::mutex> lk(m_lock);
      auto& entry = m_decorations[key];
      entry.SetKey(key); // Ensure correct type if instantiated here
      if(entry.m_state == DispositionState::PartlySatisfied ||
         entry.m_state == DispositionState::Satisfied)
        throw std::runtime_error("Cannot mark a decoration as unsatisfiable when that decoration is already present on this packet");

      // Mark the entry as permanently checked-out
      entry.m_state = DispositionState::Unsatisfiable;
    }

    // Now trigger a rescan:
    MarkUnsatisfiable(key);
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
    DecorationKey key(auto_id<T>::key());

    // Create a copy of the input, put the copy in a shared pointer
    auto ptr = std::make_shared<T>(std::forward<T&&>(t));
    Decorate(AnySharedPointer(ptr), key);
    return *ptr;
  }

  /// <summary>
  /// Decoration method specialized for shared pointer types
  /// </summary>
  /// <remarks>
  /// This decoration method has the additional benefit that it will make direct use of the passed
  /// shared pointer.
  ///
  /// If the passed value is null, the corresponding value will be marked unsatisfiable.
  /// </remarks>
  template<class T>
  void Decorate(std::shared_ptr<T> ptr) {
    DecorationKey key(auto_id<T>::key());
    
    // We don't want to see this overload used on a const T
    static_assert(!std::is_const<T>::value, "Cannot decorate a shared pointer to const T with this overload");

    // Injunction to prevent existential loops:
    static_assert(!std::is_same<T, AutoPacket>::value, "Cannot decorate a packet with another packet");
    
    // Either decorate, or prevent anyone from decorating
    if (ptr)
      Decorate(AnySharedPointer(ptr), key);
    else
      MarkUnsatisfiable(key);
  }

  /// <summary>
  /// Decoration method specialized for const shared pointer types
  /// </summary>
  /// <remarks>
  /// This decoration method has the additional benefit that it will make direct use of the passed
  /// shared pointer.
  /// </remarks>
  template<class T>
  void Decorate(std::shared_ptr<const T> ptr) {
    Decorate(std::const_pointer_cast<T>(ptr));
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
      !is_any<is_shared_ptr<T>::value, is_shared_ptr<Ts>::value...>::value,
      "DecorateImmediate must not be used to attach a shared pointer, use Decorate on such a decoration instead"
    );

    // Perform standard decoration with a short initialization:
    std::unique_lock<std::mutex> lk(m_lock);
    DecorationDisposition* pTypeSubs[1 + sizeof...(Ts)] = {
      &DecorateImmediateUnsafe(DecorationKey(auto_id<T>::key()), &immed),
      &DecorateImmediateUnsafe(DecorationKey(auto_id<Ts>::key()), &immeds)...
    };
    lk.unlock();


    // Pulse satisfaction:
    MakeAtExit([this, &pTypeSubs] {
      // Mark entries as unsatisfiable:
      // IMPORTANT: isCheckedOut = true prevents subsequent decorations of this type
      for(DecorationDisposition*  pEntry : pTypeSubs) {
        pEntry->m_pImmediate = nullptr;
        pEntry->m_state = DispositionState::Unsatisfiable;
      }

      // Now trigger a rescan to hit any deferred, unsatisfiable entries:
#if autowiring_USE_LIBCXX
      for (const std::type_info* ti : {&auto_id<T>::key(), &auto_id<Ts>::key()...})
        MarkUnsatisfiable(DecorationKey(*ti));
#else
      bool dummy[] = {
        (MarkUnsatisfiable(DecorationKey(auto_id<T>::key())), false),
        (MarkUnsatisfiable(DecorationKey(auto_id<Ts>::key())), false)...
      };
      (void)dummy;
#endif
    }),
    PulseSatisfaction(pTypeSubs, 1 + sizeof...(Ts));
  }

  /// <summary>
  /// Adds a recipient for data associated only with this issuance of the packet.
  /// </summary>
  /// <remarks>
  /// This method is not idempotent.  The returned Recipient structure may be used to remove
  /// the recipient safely at any point.  The caller MUST NOT attempt 
  /// </remarks>
  const SatCounter* AddRecipient(const AutoFilterDescriptor& descriptor);

  /// <summary>
  /// Removes a previously added packet recipient
  /// </summary>
  void RemoveRecipient(const SatCounter& recipient);

  /// <summary>
  /// Convenience overload, identical in behavior to AddRecipient
  /// </summary>
  template<class Fx>
  const SatCounter* operator+=(Fx&& fx) {
    return AddRecipient(AutoFilterDescriptor(std::forward<Fx&&>(fx)));
  }

  /// <summary>
  /// Convenience overload, provided to allow the attachment of receive-only filters to a const AutoPacket
  /// </summary>
  template<class Fx>
  const AutoPacket& operator+=(Fx&& fx) const {
    static_assert(
      !Decompose<decltype(&Fx::operator())>::template any<arg_is_out>::value,
      "Cannot add an AutoFilter to a const AutoPacket if any of its arguments are output types"
      );
    *const_cast<AutoPacket*>(this) += std::forward<Fx&&>(fx);
    return *this;
  }

  /// <returns>A reference to the satisfaction counter for the specified type</returns>
  /// <remarks>
  /// If the type is not a subscriber GetSatisfaction().GetType() == nullptr will be true
  /// </remarks>
  template<class T>
  inline const SatCounter& GetSatisfaction(void) const { return GetSatisfaction(auto_id<T>::key()); }

  /// <summary>
  /// Returns the next packet that will be issued by the packet factory in this context relative to this context
  /// </summary>
  std::shared_ptr<AutoPacket> Successor(void);

  /// <returns>True if the indicated type has been requested for use by some consumer</returns>
  template<class T>
  bool HasSubscribers(void) const {
    return HasSubscribers(DecorationKey(auto_id<T>::key()));
  }

  struct SignalStub {
    SignalStub(const AutoPacket& packet, std::condition_variable& cv) :
      packet(packet),
      cv(&cv)
    {}

    const AutoPacket& packet;
    std::condition_variable* cv;
    bool is_satisfied = false;
    bool is_complete = false;
  };

  /// <summary>
  /// Blocks until the specified descriptor is satisfied
  /// </summary>
  /// <param name="duration">
  /// The amount of time to wait.  If set to std::chrono::nanoseconds::max, this method will block indefinitely
  /// </param>
  /// <returns>False if a timeout occurred, true otherwise</returns>
  /// <remarks>
  /// This method considers the arguments described by "inputs" and will block until all decorations that are needed
  /// by this filter have been satisfied on this packet.  When this function returns, the specified filter will have
  /// been called.
  /// </remarks>
  bool Wait(std::condition_variable& cv, const AutoFilterDescriptorInput* inputs, std::chrono::nanoseconds duration = std::chrono::nanoseconds::max());

  /// <summary>
  /// Blocks until the passed lambda function can be called
  /// </summary>
  /// <param name="cv">A condition variable that can be signalled when the wait condition has expired</param>
  /// <returns>
  /// True on success, false if a timeout occurred
  /// </returns>
  /// <remarks>
  /// This method will invoke the passed autoFilter function once all of its arguments are present on the packet
  /// this routine generates internally.  This method will not return until the specified autoFilter is called.
  /// If the autoFilter cannot be called because the required decorations are not present on the packet on teardown,
  /// this method will throw an exception.
  ///
  /// The passed autoFilter routine must not attach any decorations to the packet, nor may it accept a non-const
  /// AutoFilter as an input argument.
  /// </remarks>
  template<class Fx>
  bool Wait(std::condition_variable& cv, Fx&& autoFilter, std::chrono::nanoseconds duration = std::chrono::nanoseconds::max())
  {
    auto stub = std::make_shared<SignalStub>(*this, cv);

    // Add the filter that will ultimately be invoked
    *this += std::move(autoFilter);
    return Wait(cv, Decompose<decltype(&Fx::operator())>::template Enumerate<AutoFilterDescriptorInput>::types, duration);
  }

  /// <summary>
  /// Delays until the specified decorations are satisfied
  /// </summary>
  template<class... Decorations>
  bool Wait(std::condition_variable& cv)
  {
    static const AutoFilterDescriptorInput inputs [] = {
      static_cast<auto_arg<Decorations>*>(nullptr)...,
      AutoFilterDescriptorInput()
    };

    return Wait(cv, inputs, std::chrono::nanoseconds::max());
  }

  /// <summary>
  /// Timed version of Wait
  /// </summary>
  template<class... Args>
  bool Wait(std::chrono::nanoseconds duration, std::condition_variable& cv)
  {
    static const AutoFilterDescriptorInput inputs [] = {
      static_cast<auto_arg<Args>*>(nullptr)...,
      AutoFilterDescriptorInput()
    };

    return Wait(cv, inputs, duration);
  }


  /// Get the context of this packet (The context of the AutoPacketFactory that created this context)
  std::shared_ptr<CoreContext> GetContext(void) const;
};

template<class T>
bool AutoPacket::Get(const std::shared_ptr<T>*& out) const {
  static_assert(!std::is_const<T>::value, "Overload resolution selected an incorrect version of Get");

  const DecorationDisposition* pDisposition = GetDisposition(DecorationKey(auto_id<T>::key()));
  if (!pDisposition || pDisposition->m_decorations.size() != 1) {
    out = nullptr;
    return false;
  }

  out = &pDisposition->m_decorations[0]->as_unsafe<T>();
  return true;
}
