// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _SHARED_PTR_WRAP_H
#define _SHARED_PTR_WRAP_H
#include "Object.h"
#include <map>
#include SHARED_PTR_HEADER
#include TYPE_TRAITS_HEADER

class CoreContext;
class ContextMember;
class Object;
class SharedPtrWrapBase;

namespace std {
  /// <summary>
  /// Identical to static_pointer_cast if U inherits T, dynamic_pointer_cast otherwise
  /// </summary>
  template<class T, class U>
  inline typename std::enable_if<std::is_base_of<T, U>::value, std::shared_ptr<T>>::type fast_pointer_cast(typename const std::shared_ptr<U>& Other) {
    return std::static_pointer_cast<T, U>(Other);
  };

  template<class T, class U>
  inline typename std::enable_if<
    !std::is_base_of<T, U>::value &&
    std::is_polymorphic<T>::value &&
    std::is_polymorphic<U>::value,
    std::shared_ptr<T>
  >::type fast_pointer_cast(const std::shared_ptr<U>& Other) {
    return std::dynamic_pointer_cast<T, U>(Other);
  }

  template<class T, class U>
  inline std::shared_ptr<T> fast_pointer_cast(...) {
    return std::shared_ptr<T>();
  }
}

/// <summary>
/// Base class support functionality for SharedPtrWrap in order to enable template polymorphism
/// </summary>
class SharedPtrWrapBase
{
public:
  SharedPtrWrapBase(std::weak_ptr<CoreContext> pAutowirer):
    pAutowirer(pAutowirer)
  {}
  virtual ~SharedPtrWrapBase(void) {}

  // The autowirer responsible for this pointer wrap, placed in a weak pointer
  std::weak_ptr<CoreContext> pAutowirer;

  /// <summary>
  /// Obtains the pointer information of the encapsulated type
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;

  /// <summary>
  /// Casts this shared pointer to a shared pointer to the base object
  /// </summary>
  virtual std::shared_ptr<Object> AsObject();
};

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T, bool isPoly = std::is_base_of<Object, T>::value>
class SharedPtrWrap:
  public SharedPtrWrapBase,
  public std::shared_ptr<T>
{
public:
  SharedPtrWrap(std::weak_ptr<CoreContext> pAutowirer, T* p):
    SharedPtrWrapBase(pAutowirer)
  {}

  SharedPtrWrap(std::weak_ptr<CoreContext> pAutowirer, std::shared_ptr<T>& p):
    SharedPtrWrapBase(pAutowirer),
    std::shared_ptr<T>(p)
  {}

  virtual const std::type_info& GetTypeInfo(void) const {
    return typeid(T);
  }
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
  public SharedPtrWrap<T, false>
{
public:
  SharedPtrWrap(std::weak_ptr<CoreContext> pAutowirer, T* p):
    SharedPtrWrap<T, false>(pAutowirer, p)
  {}

  SharedPtrWrap(std::weak_ptr<CoreContext> pAutowirer, std::shared_ptr<T>& p):
    SharedPtrWrap<T, false>(pAutowirer, p)
  {}

  virtual std::shared_ptr<Object> AsObject() {
    return std::static_pointer_cast<Object, T>(*this);
  }
};

#endif
