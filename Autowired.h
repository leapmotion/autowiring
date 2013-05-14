#ifndef _AUTOWIRED_H
#define _AUTOWIRED_H
#include "ocuConfig.h"
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

template<class T>
class AutowiredCreator:
  public AutowirableSlot,
  public std::shared_ptr<T>
{
public:
  typedef shared_ptr<T> t_ptrType;
  
  /// <summary>
  /// Creates a new instance if this instance isn't autowired
  /// </summary>
  /// <remarks>
  /// Users are encouraged to make use of AutoRequired wherever it's a sensible alternative
  /// to a Create call.
  /// </remarks>
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

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return t_ptrType::get();
  }

  AutowiredCreator<T>& operator=(T* rhs) {
    // Set up the shared pointer first:
    std::shared_ptr<T>::reset(rhs);

    // Strong assumption must be made, here, that the rhs isn't already in the current context
    LockContext()->Add(*this);
    return *this;
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}
};

template<>
class AutowiredCreator<CoreContext>:
  public AutowirableSlot,
  public std::shared_ptr<CoreContext>
{
public:
  typedef shared_ptr<CoreContext> t_ptrType;
  void Create(void);

  using std::shared_ptr<CoreContext>::operator=;
  bool IsAutowired(void) const override {return !!t_ptrType::get();}
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
class AutowiredCreator<GlobalCoreContext>:
  public AutowirableSlot,
  public std::shared_ptr<GlobalCoreContext>
{
private:
  // We do not allow operator=
  using std::shared_ptr<GlobalCoreContext>::operator=;

public:
  typedef shared_ptr<GlobalCoreContext> t_ptrType;
  
  AutowiredCreator(void):
    std::shared_ptr<GlobalCoreContext>(GetGlobalContext())
  {
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}

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
  public AutowiredCreator<T>
{
public:
  Autowired(void) {
    shared_ptr<CoreContext> context = AutowirableSlot::LockContext();
    context->Autowire(*this);
  }

  Autowired(T* ptr) {
    *this = ptr;
  }

  using AutowiredCreator<T>::operator=;
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
  public AutowiredCreator<CoreContext>
{
public:
  /// <remarks>
  /// Specialized constructor that enables the forced creation of a new child context
  /// </remarks>
  /// <param name="forceNew">Set if a new context is required</param>
  Autowired(bool forceNew = false);
  
  using AutowiredCreator<CoreContext>::operator=;
};

template<>
class Autowired<GlobalCoreContext>:
  public AutowiredCreator<GlobalCoreContext>
{};

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
      AutowiredCreator<T>::Create();
  }
};

/// <summary>
/// A special templated type that allows users to specify factory construction methods
/// </summary>
template<class T, T* (*fn)()>
struct CtorProxy {};

/// <summary>
/// An AutoRequired specialization that allows the user to specify a function call to initialize this field
/// </summary>
/// <remarks>
/// This specialization is useful when it's necessary to AutoRequire an interface
/// </remarks>
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
/// A special templated type that allows users to specify a particular concrete instance
/// </summary>
template<class T, class Concrete>
struct CtorConcrete {};

template<class T, class Concrete>
class AutoRequired< CtorConcrete<T, Concrete> >:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(*this)
      return;

    this->reset(new Concrete);
    AutowirableSlot::LockContext()->Add(*this);
  }
};

#endif
