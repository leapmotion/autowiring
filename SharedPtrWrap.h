#pragma once
#include "Object.h"
#include <memory>
#include <typeinfo>

/// <summary>
/// Base class support functionality for SharedPtrWrap in order to enable template polymorphism
/// </summary>
class SharedPtrWrapBase
{
public:
  virtual ~SharedPtrWrapBase(void);

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
  SharedPtrWrapImpl(T* p):
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

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T, bool isObject = cpp11::is_base_of<Object, T>::value> class SharedPtrWrap;

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T>
class SharedPtrWrap<T, false>:
  public SharedPtrWrapImpl<T>
{
public:
  SharedPtrWrap(T* p):
    SharedPtrWrapImpl<T>(p)
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
  SharedPtrWrap(T* p):
    SharedPtrWrapImpl<T>(p)
  {}

  virtual cpp11::shared_ptr<Object> AsObject() {
    return
      cpp11::is_base_of<Object, T>::value ?
      cpp11::static_pointer_cast<Object, T>(*this) :
      cpp11::shared_ptr<Object>();
  }
};

