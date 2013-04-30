#ifndef _AUTOWIRED_H
#define _AUTOWIRED_H
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include "InstantiatorLink.h"
#include <functional>
#include <memory>

template<class T>
class Autowired;
class CoreContext;
class GlobalCoreContext;

// Redeclarations, primary declarations in CoreContext.h
std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
std::shared_ptr<CoreContext> GetGlobalContextAsCoreContext(void);
std::shared_ptr<CoreContext> GetCurrentContext(void);
void AddGlobalObjects(InstantiatorLink* pLink);

// Exists only to get around a header cyclic dependency, needed only because we
// cannot forward-declare member functions more than once
extern std::shared_ptr<CoreContext> NewContextThunk(void);
extern std::shared_ptr<CoreContext> NewContextThunk(std::shared_ptr<CoreContext>& pParent);

template<class T, bool isAbstract>
class AutowiredCreator:
  public AutowirableSlot,
  public std::shared_ptr<T>
{
public:
  typedef shared_ptr<T> t_ptrType;

  /// <summary>
  /// This creates a pointer using the specified lambda, if such creation is needed.
  /// <summary>
  void Create(const std::function<T* ()>& fn) {
    // Is the object already created?  Short-circuit if so.
    if(*this)
      return;

    // Okay, we're ready to go now, we can release
    // the shared pointer so any lambdas disappear
    AutowirableSlot::m_tracker = std::shared_ptr<AutowirableSlot>();
    
    // TODO:  Allow this to be lazily invoked
    // It would be nice if this constructor is only invoked on the first dereference
    // of this autowired object.  That would allow us to specify default types that
    // aren't constructed spuriously.
    T* ptr = fn();

    // Now we'll add this object to the context so the created object may be autowired elsewhere.
    // We also want to be sure we use the same shared_ptr that's being used internally in the
    // context.
    std::shared_ptr<CoreContext> context = LockContext();
    *this = context->Add(ptr);
  }

  bool IsAutowired(void) const override {return !!this->get();}

  operator bool(void) const {
    return !!t_ptrType::get();
  }

  operator T*(void) const {
    return t_ptrType::get();
  }

  using std::shared_ptr<T>::operator=;
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
    this->reset(new T);
    AutowirableSlot::LockContext()->Add(*this);
  }

  using AutowiredCreator<T, true>::operator=;
};

template<>
class AutowiredCreator<CoreContext, false>:
  public AutowirableSlot,
  public std::shared_ptr<CoreContext>
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

  using std::shared_ptr<CoreContext>::operator=;
  bool IsAutowired(void) const override {return !!this->get();}
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
  public AutowirableSlot,
  public std::shared_ptr<GlobalCoreContext>
{
private:
  // We do not allow operator=
  using std::shared_ptr<GlobalCoreContext>::operator=;

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
  static InstantiatorLink* MakeLink(void) {
    static InstantiatorLink link = {nullptr, &MakeInstance<W>};
    return &link;
  }

  AutowiredCreator(void):
    std::shared_ptr<GlobalCoreContext>(GetGlobalContext())
  {
  }

  bool IsAutowired(void) const override {return !!this->get();}

  operator GlobalCoreContext*(void) const {
    return t_ptrType::get();
  }
};

/// <summary>
/// An autowired template class that forms the foundation of the context consumer system
/// </summary>
/// <param name="T">The class whose type is to be found.  Must be an EXACT match.</param>
/// <remarks>
/// The autowired class offers a quick way to import or create an instance of a specified
/// class in the local context.
///
/// This class may be safely used even when the member in question is an abstract type.
/// </remarks>
template<class T>
class Autowired:
  public AutowiredCreator<T, false>
{
public:
  Autowired(void) {
    shared_ptr<CoreContext> context = AutowirableSlot::LockContext();
    context->Autowire(*this);
  }
};

/// <summary>
/// Autowiring specialization that does not do a context membership search
/// </summary>
/// <remarks>
/// This specialization is guaranteed to be identical to obtaining a shared_ptr
/// reference to the result of CoreContext::CurrentContext.  It is provided for
/// symmetry of declaration.
/// </remarks>
template<>
class Autowired<CoreContext>:
  public AutowiredCreator<CoreContext, false>
{
public:
  /// <remarks>
  /// Specialized constructor that enables the forced creation of a new child context
  /// </remarks>
  /// <param name="forceNew">Set if a new context is required</param>
  Autowired(bool forceNew = false);

  using std::shared_ptr<CoreContext>::operator=;
};

template<>
class Autowired<GlobalCoreContext>:
  public AutowiredCreator<GlobalCoreContext, false>
{};

template<class T, T* (*fn)()>
struct CtorProxy {};

/// <summary>
/// Similar to Autowired, but the default constructor invokes Autowired(true)
/// </summary>
/// <remarks>
/// This class is simply a convenience class and provides a declarative way to name a
/// required dependency.
/// </remarks>
template<class T>
class AutoRequired:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(!*this)
      AutowiredCreator<T, false>::Create();
  }
};

template<class T, T* (*fn)()>
class AutoRequired< CtorProxy<T, fn> >:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(*this)
      return;
    
    this->reset(fn());
    AutowirableSlot::LockContext()->Add(*this);
  }
};

/// <summary>
/// Provides a way to name a dependency on a specific instance of a class
/// </summary>
/// <remarks>
/// 
/// </remarks>
template<class T, const char* name>
class AutoNamed:
  public Autowired<T>
{
public:
};

/// <summary>
/// Autofind is like autowired, except it holds a weak reference to the matched class
/// </summary>
template<class T>
class AutowiredWeak:
  public std::weak_ptr<T>,
  public AutowirableSlot
{
public:
  // TODO: Fix this, we need a way to autowire weak pointers
  AutowiredWeak(void):
      std::weak_ptr<T>()
  {
  }
};

#endif
