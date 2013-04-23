#pragma once
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
  cpp11::weak_ptr<ContextMember> m_self;
  const char* m_name;

public:
  virtual ~ContextMember();

protected:
  // Member variables:
  cpp11::weak_ptr<CoreContext> m_context;

public:
  // Accessor methods:
  const char* GetName(void) const {return m_name;}

  // Mutator methods:
  void SetContext(cpp11::shared_ptr<CoreContext>& context) {
    m_context = context;
  }

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
  cpp11::shared_ptr<CoreContext> GetContext(void) const {return m_context.lock();}

  /// <summary>
  /// Returns a shared pointer that refers to ourselves
  /// </summary>
  template<class T>
  cpp11::shared_ptr<T> GetSelf(void) {
    // This exists exclusively to ensure casting safety
    static_cast<T*>((ContextMember*)nullptr);

    // HACK:  This is a stupid way to get a shared pointer to the type we want, but I don't see another way.
    return (cpp11::shared_ptr<T>&)m_self.lock();
  }
};

