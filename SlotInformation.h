#pragma once
#include "TypeUnifier.h"
#include <typeinfo>
#include MEMORY_HEADER

class DeferrableAutowiring;

/// <summary>
/// Represents information about a single slot detected as having been declared in a context member
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
struct SlotInformationStumpBase {
  SlotInformationStumpBase(void) :
    bInitialized(false),
    pHead(nullptr)
  {}

  // Initialization flag, used to indicate that this stump has valid data
  bool bInitialized;

  // Current slot information:
  const SlotInformation* pHead;
};

/// <summary>
/// Utility type, inherits true_type if T should use the type unifier
/// </summary>
template<class T>
struct use_unifier:
  std::integral_constant<
    bool,
    !std::is_void<T>::value &&
    !std::is_scalar<T>::value &&
    !std::is_base_of<Object, T>::value
  >
{};

template<class T, bool needsUnifier = use_unifier<T>::value>
struct SlotInformationStump;

template<class T>
struct SlotInformationStump<T, false>:
  SlotInformationStumpBase
{
  static SlotInformationStump s_stump;
};

/// <summary>
/// Specializations for types which will be using the type unifier
/// </summary>
template<class T>
struct SlotInformationStump<T, true>
{
  static SlotInformationStump<typename SelectTypeUnifier<T>::type>& s_stump;
};

template<class T>
SlotInformationStump<T, false> SlotInformationStump<T, false>::s_stump;

template<class T>
SlotInformationStump<typename SelectTypeUnifier<T>::type>& SlotInformationStump<T, true>::s_stump = SlotInformationStump<typename SelectTypeUnifier<T>::type>::s_stump;

/// <summary>
/// A stack location linked list, stored in a per-thread basis and used to track slots
/// </summary>
class SlotInformationStackLocation {
private:
  SlotInformationStackLocation(const SlotInformationStackLocation& rhs) = delete;
  SlotInformationStackLocation(SlotInformationStackLocation&& rhs) :
    m_pPrior(rhs.m_pPrior),
    m_pStump(rhs.m_pStump),
    m_pCur(rhs.m_pCur),
    m_pObj(rhs.m_pObj),
    m_extent(rhs.m_extent)
  {
    rhs.m_pStump = nullptr;
  }
  SlotInformationStackLocation(SlotInformationStumpBase* pStump, const void* pObj = nullptr, size_t extent = 0);

public:
  ~SlotInformationStackLocation(void);

private:
  // The prior stack location, made current when this slot goes out of scope
  SlotInformationStackLocation* m_pPrior;

  // The pointer location where the stump will be stored:
  SlotInformationStumpBase* m_pStump;

  // Current slot information:
  SlotInformation* m_pCur;

  // Information about the object being constructed while this stack location is valid:
  const void* m_pObj;
  size_t m_extent;

public:
  /// <returns>
  /// True if the passed pointer is inside of the object currently under construction at this stack location
  /// </returns>
  bool Encloses(const void* ptr) {
    return m_pObj < ptr && ptr < (char*) m_pObj + m_extent;
  }

  /// <summary>
  /// The slot information stump for this stack location
  /// </summary>
  SlotInformationStumpBase* GetStump(void) const {
    return m_pStump;
  }

  /// <returns>
  /// Creates a new stack location which remains current as long as the return type is not destroyed
  /// </returns>
  /// <param name="pSpace">The pointer to the base of the space about to be constructed</param>
  template<class T>
  static SlotInformationStackLocation PushStackLocation(T* pSpace) {
    auto* pStump = &SlotInformationStump<T>::s_stump;

    // If we're already initialized, then we have nothing to do.  This line is an optimization; if there
    // is a race here, then the worst-case scenario is an unneeded sequence of memory allocations that
    // will only ever be referenced by this stack location.
    if(pStump->bInitialized)
      return SlotInformationStackLocation(pStump);

    // New stack location to enclose this stump.  This stack location may be concurrent with respect
    // to other threads, but only one thread will succeed in colonizing this stump with a chain of
    // slot entries
    return SlotInformationStackLocation(pStump, pSpace, sizeof(T));
  }

  /// <summary>
  /// Returns the current information stump entry
  /// </summary>
  static SlotInformationStackLocation* CurrentStackLocation(void);

  /// <summary>
  /// Returns the stump in the current stack location, or null
  /// </summary>
  static SlotInformationStumpBase* CurrentStump(void);

  /// <summary>
  /// Registers the named slot with the current stack location
  /// </summary>
  /// <remarks>
  static void RegisterSlot(DeferrableAutowiring* pAutowiring);

  // Operator overloads:
  void operator=(SlotInformationStackLocation&& rhs) = delete;
  void operator=(const SlotInformationStackLocation& rhs) = delete;
};
