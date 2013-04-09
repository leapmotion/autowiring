#pragma once
#include "Object.h"
#include <map>
#include <memory>
#include <typeinfo>

class Autowirer;
class ContextMember;
class Object;
class SharedPtrWrapBase;

/// <summary>
/// Base class support functionality for SharedPtrWrap in order to enable template polymorphism
/// </summary>
class SharedPtrWrapBase
{
public:
  SharedPtrWrapBase(cpp11::weak_ptr<Autowirer> pAutowirer):
    pAutowirer(pAutowirer)
  {}
  virtual ~SharedPtrWrapBase(void) {}

  // The autowirer responsible for this pointer wrap, placed in a weak pointer
  cpp11::weak_ptr<Autowirer> pAutowirer;

  /// <summary>
  /// Obtains the pointer information of the encapsulated type
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;

  /// <summary>
  /// Casts this shared pointer to a shared pointer to the base object
  /// </summary>
  virtual cpp11::shared_ptr<Object> AsObject();
};

/// <summary>
/// Helper class for the two cases of shared ptr--inherited from Object, and not. 
/// </summary>
template<class T>
class SharedPtrWrapImpl:
  public SharedPtrWrapBase,
  public cpp11::shared_ptr<T>
{
public:
  SharedPtrWrapImpl(cpp11::weak_ptr<Autowirer> pAutowirer, T* p):
    SharedPtrWrapBase(pAutowirer),
    cpp11::shared_ptr<T>(p)
  {}

  ~SharedPtrWrapImpl(void) {
    // The existence of this virtual function will ensure that the
    // correct version of shared_ptr's destructor gets called.
  }

  virtual const std::type_info& GetTypeInfo(void) const {
    return typeid(T);
  }
};

template<class T, bool isPoly = cpp11::is_base_of<Object, T>::value>
class SharedPtrWrap;

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T>
class SharedPtrWrap<T, false>:
  public SharedPtrWrapImpl<T>
{
public:
  SharedPtrWrap(cpp11::weak_ptr<Autowirer> pAutowirer, T* p):
    SharedPtrWrapImpl<T>(pAutowirer, p)
  {}
};

/// <summary>
/// Specialization for classes that inherit from Object
/// </summary>
/// <remarks>
/// Classes which inherit from Object are candidates for filling in abstract Autowired
/// components.  Without Object, there is no common point of exchange and no single
/// known type that both the wrap and the search methods can agree upon as a unified
/// pointer exchange location.
/// </remarks>
template<class T>
class SharedPtrWrap<T, true>:
  public SharedPtrWrapImpl<T>
{
public:
  SharedPtrWrap(cpp11::weak_ptr<Autowirer> pAutowirer, T* p):
    SharedPtrWrapImpl<T>(pAutowirer, p)
  {}

  virtual cpp11::shared_ptr<Object> AsObject() {
    return cpp11::static_pointer_cast<Object, T>(*this);
  }
};

