#pragma once
#include "Object.h"
#include "TeardownNotifier.h"
#include MEMORY_HEADER

class CoreContext;
struct SlotInformation;
struct SlotInformationStump;

/// <summary>
/// A class that must be inherited in order to be a member of a context heriarchy
/// </summary>
class ContextMember:
  public Object,
  public TeardownNotifier,
  public std::enable_shared_from_this<ContextMember>
{
protected:
  ContextMember(const char* name = nullptr);

public:
  virtual ~ContextMember();

protected:
  // Pointer back to the enclosing context
  const std::weak_ptr<CoreContext> m_context;

  // Name of this Contextmember:
  const char* m_name;

  // Slots defined on this type, updated as the base type is constructed
  const SlotInformationStump* m_pSlots;

public:
  // Accessor methods:
  const char* GetName(void) const {return m_name;}
  bool IsOrphaned(void) const {return m_context.expired();}

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
  /// The linked list is guaranteed to be in reverse-sorted order
  /// </remarks>
  const SlotInformation* GetSlotInformation(void) const;

  /// <summary>
  /// This method is invoked after all embedded Autowired members of this class are initialized
  /// </summary>
  /// <remarks>
  /// Not currently implemented
  /// </remarks>
  virtual void PostConstruct(void) {}

  /// <summary>
  /// Invoked by the parent context when the parent context is about to be destroyed
  /// </summary>
  /// <remarks>
  /// A context may be destroyed if and only if none of its members are running and none of
  /// them may enter a runnable state.  This happens when the last pointer to ContextMember
  /// is lost.  Resource cleanup must be started at this point.
  ///
  /// For contexts containing strictly heirarchial objects, implementors of this method do
  /// not need to do anything.  If, however, there are circular references anywhere in the
  /// context, callers should invoke reset() on each autowired member they control.
  ///
  /// TODO:  Eventually, we MUST eliminate this method and simply keep track of all Autowired
  /// instances in a single context.  This can be done safely by using the Autowired's internal
  /// m_tracker member, which can serve to notify listeners when the instance is destroyed.
  /// Alternatively, the Autowired instance could attach and detach itself from a linked list
  /// in a lock-free way in order to support chain detachment.
  /// </remarks>
  virtual void NotifyContextTeardown(void) {}

  /// <summary>
  /// Retrieves the context associated with this object.
  /// </summary>
  /// <remarks>
  /// By default, the context will be whatever the current context was in the thread
  /// where this object was constructed at the time of the object's construction.
  ///
  /// Note that, if the context is in the process of tearing down, this return value
  /// could be null.
  /// </remarks>
  std::shared_ptr<CoreContext> GetContext(void) const { return m_context.lock(); }

  /// <summary>
  /// Returns a shared pointer that refers to ourselves
  /// </summary>
  template<class T>
  std::shared_ptr<T> GetSelf(void) {
    return std::static_pointer_cast<T, ContextMember>(shared_from_this());
  }
};

