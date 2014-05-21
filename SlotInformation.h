#pragma once
#include "AutowirableSlot.h"
#include <typeinfo>
#include MEMORY_HEADER

class ContextMember;
class DeferrableAutowiring;

/// <summary>
/// Represents information about a single slot detected as having been declared in a ContextMember
/// </summary>
struct SlotInformation {
  SlotInformation(const SlotInformation* pFlink, const std::type_info& type, size_t slotOffset, bool autoRequired) :
    pFlink(pFlink),
    type(type),
    slotOffset(slotOffset),
    autoRequired(autoRequired)
  {}

  // The next slot defined on this type
  const SlotInformation* const pFlink;

  // The type of this slot:
  const std::type_info& type;

  // The offset of this slot relative to the base of the enclosing object
  size_t slotOffset;

  // True if this slot was AutoRequired
  bool autoRequired;
};

/// <summary>
/// Stump entry, used to anchor a chain of slot information entries
/// </summary>
struct SlotInformationStump {
  SlotInformationStump(void) :
    bInitialized(false),
    pHead(nullptr)
  {}
  ~SlotInformationStump(void);

  // Initialization flag, used to indicate that this stump has valid data
  bool bInitialized;

  // Current slot information:
  SlotInformation* pHead;
};

/// <summary>
/// A stack location linked list, stored in a per-thread basis and used to track slots
/// </summary>
class SlotInformationStackLocation {
private:
  SlotInformationStackLocation(const SlotInformationStackLocation& rhs) = delete;
  SlotInformationStackLocation(SlotInformationStackLocation&& rhs) = delete;
  SlotInformationStackLocation(SlotInformationStump* pStump = nullptr, const void* pObj = nullptr, const void* pContextMember = nullptr);

public:
  ~SlotInformationStackLocation(void);

private:
  // The prior stack location, made current when this slot goes out of scope
  SlotInformationStackLocation* m_pPrior;

  // The pointer location where the stump will be stored:
  SlotInformationStump* m_pStump;

  // Current slot information:
  SlotInformation* m_pCur;

  // Information about the object being constructed while this stack location is valid:
  const void* m_pObj;
  const void* m_pContextMember;

public:
  /// <returns>
  /// The slot information stump for the specified type
  /// </returns>
  template<class T>
  static SlotInformationStump* GetStump(void) {
    static std::unique_ptr<SlotInformationStump> s_pSlot(new SlotInformationStump);
    return s_pSlot.get();
  }

  /// <summary>
  /// Default method, does nothing
  /// </summary>
  template<class T>
  static void PushStackLocation(void*) {}

  /// <returns>
  /// Creates a new stack location which remains current as long as the return type is not destroyed
  /// </returns>
  /// <param name="pSpace">The pointer to the base of the space about to be constructed</param>
  /// <param name="contextMemberOffset">The offset between the base of the space and the ContextMember field</param>
  template<class T>
  static SlotInformationStackLocation PushStackLocation(ContextMember* pContextMember) {
    T* pObj = static_cast<T*>(pContextMember);
    auto* pStump = GetStump<T>();

    // If we're already initialized, then we have nothing to do.  This line is an optimization; if there
    // is a race here, then the worst-case scenario is an unneeded sequence of memory allocations that
    // will only ever be referenced by this stack location.
    if(pStump->bInitialized)
      return SlotInformationStackLocation(pStump);

    // New stack location to enclose this stump.  This stack location may be concurrent with respect
    // to other threads, but only one thread will succeed in colonizing this stump with a chain of
    // slot entries
    return SlotInformationStackLocation(pStump, pObj, static_cast<ContextMember*>(pObj));
  }

  /// <summary>
  /// Returns the current information stump entry
  /// </summary>
  static SlotInformationStump* CurrentStackLocation(void);

  /// <summary>
  /// Registers the named slot with the current stack location
  /// </summary>
  /// <remarks>
  static void RegisterSlot(DeferrableAutowiring* pAutowiring);

  // Operator overloads:
  void operator=(SlotInformationStackLocation&& rhs) = delete;
  void operator=(const SlotInformationStackLocation& rhs) = delete;
};
