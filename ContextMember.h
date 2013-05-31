#ifndef _CONTEXT_MEMBER_H
#define _CONTEXT_MEMBER_H
#include "Object.h"
#include <memory>

class CoreContext;

/// <summary>
/// A class that must be inherited in order to be a member of a context heriarchy
/// </summary>
class ContextMember:
  public Object
{
protected:
  ContextMember(const char* name = nullptr);
  std::weak_ptr<ContextMember> m_self;
  const char* m_name;

public:
  virtual ~ContextMember();

  static void* operator new(size_t nBytes);

protected:
  // Member variables:
  std::weak_ptr<CoreContext> m_context;

public:
  // Accessor methods:
  const char* GetName(void) const {return m_name;}

  // Mutator methods:
  void SetContext(std::shared_ptr<CoreContext>& context) {
    m_context = context;
  }

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
  virtual void ReleaseAll(void) {}

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
  std::shared_ptr<CoreContext> GetContext(void) const {return m_context.lock();}

  /// <summary>
  /// Returns a shared pointer that refers to ourselves
  /// </summary>
  template<class T>
  std::shared_ptr<T> GetSelf(void) {
    // This exists exclusively to ensure casting safety
    static_cast<T*>((ContextMember*)nullptr);

    // HACK:  This is a stupid way to get a shared pointer to the type we want, but I don't see another way.
    return (std::shared_ptr<T>&)m_self.lock();
  }
};

#endif
