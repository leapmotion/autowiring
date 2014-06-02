#pragma once
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

template<class T>
struct SlotInformationStump:
  SlotInformationStumpBase
{
  /// <summary>
  /// Used to obtain a list of slots defined on this type, for reflection purposes
  /// </summary>
  /// <returns>
  /// A pointer to the head of a linked list of slots on this context member
  /// </returns>
  /// <remarks>
  /// A slot is an Autowired field defined within a specific type.  Slots are of particular
  /// interest because they denote a compile-time relationship between two types, and generally
  /// are one way to understand class relationships in a system.  Furthermore, because of their
  /// compile-time nature, they are declarative and therefore denote a relationship between
  /// types, rather than states, which makes it easier to understand how slots are linked.
  ///
  /// The returned value is cached, and should not be freed or modified as it may be in use
  /// in other parts of the program.  The behavior of this method is undefined if it's called
  /// on an object before the object is fully constructed (for instance, if the method is
  /// invoked from a constructor).  This method will return correct results even if the
  /// ContextMember type was not the first inherited type.
  ///
  /// If this method returns a correct result at any point, then all subsequent calls to this
  /// method are guaranteed to return correct results, even in the aforementioned case where
  /// the method is called during construction.  This method is guaranteed to return correct
  /// results after the first instance of a concrete type is constructed.
  ///
  /// This list is guaranteed not to contain any AutowiredFast fields defined in the class.
  ///
  /// The linked list is guaranteed to be in reverse-sorted order
  /// </remarks>
  static SlotInformationStump s_stump;
};

template<class T>
SlotInformationStump<T> SlotInformationStump<T>::s_stump;

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
