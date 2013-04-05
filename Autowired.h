#pragma once
#include "CoreContext.h"
#include <functional>
#include <memory>

template<class T, const char* pszName>
class Autowired;
class CoreContext;
class GlobalCoreContext;

// Redeclarations, primary declarations in CoreContext.h
cpp11::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
cpp11::shared_ptr<CoreContext> GetGlobalContextAsCoreContext(void);
cpp11::shared_ptr<CoreContext> GetCurrentContext(void);
void FillWithDefaults(void (*instantiator)(void));

// Exists only to get around a header cyclic dependency, needed only because we
// cannot forward-declare member functions more than once
extern cpp11::shared_ptr<CoreContext> NewContextThunk(void);
extern cpp11::shared_ptr<CoreContext> NewContextThunk(cpp11::shared_ptr<CoreContext>& pParent);

// Utility routine, for users who need a function that does nothing
template<class T>
void NullOp(T) {}

class DestroyTracker {
public:
  DestroyTracker();
  
  /// <summary>
  /// This is a destroy self-reference.  There should only ever be one shared pointer
  /// to this object, and it's located here.  Everything else should be weak pointers
  /// whose purpose is to be notified when this tracker goes away.
  /// </summary>
  cpp11::shared_ptr<DestroyTracker> m_tracker;

  /// <summary>
  /// This is the context that was available at the time the autowiring was performed.
  /// </summary>
  cpp11::shared_ptr<CoreContext> m_context;
};

template<class T, bool isAbstract> class AutowiredCreator;

template<class T>
class AutowiredCreator<T, true>:
  public DestroyTracker,
  public cpp11::shared_ptr<T>
{
public:
  typedef shared_ptr<T> t_ptrType;

  /// <summary>
  /// This creates a pointer using the specified lambda, if such creation is needed.
  /// <summary>
  void Create(const cpp11::function<T* ()>& fn) {
    // Is the object already created?  Short-circuit if so.
    if(*this)
      return;

    // Okay, we're ready to go now, we can release
    // the shared pointer so any lambdas disappear
    DestroyTracker::m_tracker = cpp11::shared_ptr<DestroyTracker>();
    
    // TODO:  Allow this to be lazily invoked
    // It would be nice if this constructor is only invoked on the first dereference
    // of this autowired object.  That would allow us to specify default types that
    // aren't constructed spuriously.
    T* ptr = fn();

    // Now we'll add this object to the context so the created object may be autowired elsewhere.
    // We also want to be sure we use the same shared_ptr that's being used internally in the
    // context.
    *this = DestroyTracker::m_context->Add(ptr);
  }

  using cpp11::shared_ptr<T>::operator=;
};

template<>
class AutowiredCreator<CoreContext, false>:
  public DestroyTracker,
  public cpp11::shared_ptr<CoreContext>
{
public:
  typedef shared_ptr<CoreContext> t_ptrType;
  void Create(void);

  /// <summary>
  /// Creates a dependent context based on the currently bound context
  /// </summary>
  void Push();

  /// <summary>
  /// Moves this context pointer up one notch to its parent context
  /// </summary>
  /// <remarks>
  /// This may cause the current pointer to become null, if the current
  /// context is the global context.
  /// </remarks>
  void Pop();

  using cpp11::shared_ptr<CoreContext>::operator=;
};

struct EmptyContext {};

/// <summary>
/// A container structure used to specify the types
/// </summary>
template<class S = EmptyContext>
struct GlobalContextName {};

enum eGlobalBehavior {
  // Simply autowire a global object in
  eDefaultGlobalBehavior,

  // Initialize, if not already initialized
  eInitGlobalBehavior
};

/// <summary>
/// This is the specialization for global contexts.  Unlike other autowires, it's guaranteed
/// to autowire in all circumstances.
/// </summary>
/// <remarks>
/// We do not autowire operator=, because there is never a case where the rhs is anything
/// but the sole Global context or null.
/// </remarks>
template<>
class AutowiredCreator<GlobalCoreContext, false>:
  public DestroyTracker,
  public cpp11::shared_ptr<GlobalCoreContext>
{
private:
  // We do not allow operator=
  using cpp11::shared_ptr<GlobalCoreContext>::operator=;

public:
  typedef shared_ptr<GlobalCoreContext> t_ptrType;
  
  /// <summary>
  /// Alias of GlobalCoreContext::InstantiateW
  /// </summary>
  template<class W>
  static void MakeInstance() {
    W s;
  }

  template<class W>
  AutowiredCreator(GlobalContextName<W> name, eGlobalBehavior behavior = eDefaultGlobalBehavior):
    cpp11::shared_ptr<GlobalCoreContext>(GetGlobalContext())
  {
    DestroyTracker::m_context = GetGlobalContextAsCoreContext();
    ASSERT(*this);
    if(behavior == eInitGlobalBehavior)
      // FillGlobalContext is idempotent
      FillWithDefaults(&MakeInstance<W>);
  }
};

template<class T>
class AutowiredCreator<T, false>:
  public AutowiredCreator<T, true>
{
public:
  using AutowiredCreator<T, true>::Create;
  static const bool s_isAbstract = false;
  
  // TODO:  This create method, and all other Autowired create methods, should also be
  // available directly as an autowired behavior.
  void Create(void) {
    if(*this)
      return;
    
    // Create and add to the context:
    // NOTE: If you are getting an error tracked to this line, ensure that class T is totally
    // defined at the point where the Autowired instance is constructed.  Generally,
    // such errors are tracked to missing header files.
    *this = DestroyTracker::m_context->Add(new T);
  }

  using AutowiredCreator<T, true>::operator=;
};

/// <summary>
/// An autowired template class that forms the foundation of the context consumer system
/// </summary>
/// <param name="T">The class whose type is to be found.  Must be an EXACT match.</param>
/// <param name="pszName">The name of the instance, if more than one instance of T exists in the context</param>
/// <remarks>
/// The autowired class offers a quick way to import or create an instance of a specified
/// class in the local context.
///
/// This class may be safely used even when the member in question is an abstract type.
/// </remarks>
template<class T, const char* pszName>
class Autowired:
  public AutowiredCreator<T, false>
{
public:
  Autowired(void) {
    DestroyTracker::m_context->Autowire(*this);
  }
};

template<const char* pszName>
class Autowired<GlobalCoreContext, pszName>:
  public AutowiredCreator<GlobalCoreContext, false>
{
public:
  /// <summary>
  /// Global context specialized constructor
  /// </summary>
  /// <remarks>
  /// Unlike other autowired instances, this one guarantees that it will be autowired,
  /// and also allows the default global context to be filled with entities based on
  /// the desired behavior.
  ///
  /// Note that this class will _not_ invoke the InitiateCoreThreads method.  This is
  /// left up to the caller.
  /// </remarks>
  template<class S>
  Autowired(GlobalContextName<S> name, eGlobalBehavior behavior = eDefaultGlobalBehavior):
    AutowiredCreator<GlobalCoreContext, false>(name, behavior)
  {}
};

/// <summary>
/// Similar to Autowired, but the default constructor invokes Autowired(true)
/// </summary>
/// <remarks>
/// This class is simply a convenience class and provides a declarative way to name a
/// required dependency.
/// </remarks>
template<class T, const char* pszName = sc_emptyName>
class AutoRequired:
  public Autowired<T, pszName>
{
public:
  AutoRequired(void)
  {
    if(!*this)
      AutowiredCreator<T, false>::Create();
  }
};

/// <summary>
/// A utility class for autowiring classes without first including their header files
/// </summary>
/// <param name="isPolymorphic">Set to true if the class is known to inherit from Object</param>
/// <remarks>
/// This is a utility class for users who _know_ that the class they need to autowire will
/// be concrete.  By using it, users do not need to first include the header file defining
/// the class--without it, excessive header dependence may result
///
/// Be forewarned:  Setting the value of isPolymorphic incorrectly can have serious consequences
/// and, though the autowiring system will detect and warn you of an incorrect assignment,
/// this will only happen when autowiring is attempted.  This unfortunate circumstance is
/// due to the fact that the autowired declaration won't have access to the full class
/// definition at declaration time, and so it won't know _anything_ about the class you are
/// trying to autowire.
/// </remarks>
template<class T, bool isPolymorphic, bool isAbstract>
class AutowiredNoForwardDeclaration:
  public AutowiredCreator<T, isAbstract>
{
public:
  static const bool s_isPolymorphic = isPolymorphic;
  AutowiredNoForwardDeclaration(void) {
    DestroyTracker::m_context->Autowire(*this);
  }
};

template<class T, bool isPolymorphic>
class AutowiredConcrete:
  public AutowiredNoForwardDeclaration<T, isPolymorphic,  false>
{};

template<class T, bool isPolymorphic>
class AutowiredAbstract:
  public AutowiredNoForwardDeclaration<T, isPolymorphic,  true>
{};

/// <summary>
/// Autofind is like autowired, except it holds a weak reference to the matched class
/// </summary>
template<class T, const char* pszName>
class AutowiredWeak:
  public cpp11::weak_ptr<T>,
  public DestroyTracker
{
public:
  // TODO: Fix this, we need a way to autowire weak pointers
  AutowiredWeak(void):
      cpp11::weak_ptr<T>()
  {
  }
};

