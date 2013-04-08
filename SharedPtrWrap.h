#pragma once
#include "Object.h"
#include <map>
#include <memory>
#include <typeinfo>

class Autowirer;
class ContextMember;
class Object;
class SharedPtrWrapBase;

template<class Mp>
void AutowirerErase(Autowirer* pAutowirer, typename Mp::iterator q);

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
template<class T, class Mp>
class SharedPtrWrapImpl:
  public SharedPtrWrapBase,
  public cpp11::shared_ptr<T>
{
public:
  typedef typename Mp::iterator myIter;
  SharedPtrWrapImpl(cpp11::weak_ptr<Autowirer> pAutowirer, T* p, myIter q):
    SharedPtrWrapBase(pAutowirer),
    cpp11::shared_ptr<T>(p)
  {}

  ~SharedPtrWrapImpl(void) {
    // The existence of this virtual function will ensure that the
    // correct version of shared_ptr's destructor gets called.
  }

  myIter m_q;

  virtual const std::type_info& GetTypeInfo(void) const {
    return typeid(T);
  }
};

template<class T, class Mp, bool isPoly = cpp11::is_base_of<Object, T>::value>
class SharedPtrWrap;

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T, class Mp>
class SharedPtrWrap<T, Mp, false>:
  public SharedPtrWrapImpl<T, Mp>
{
public:
  SharedPtrWrap(cpp11::weak_ptr<Autowirer> pAutowirer, T* p, myIter q):
    SharedPtrWrapImpl<T, Mp>(pAutowirer, p, q)
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
template<class T, class Mp>
class SharedPtrWrap<T, Mp, true>:
  public SharedPtrWrapImpl<T, Mp>
{
public:
  SharedPtrWrap(cpp11::weak_ptr<Autowirer> pAutowirer, T* p, myIter q):
    SharedPtrWrapImpl<T, Mp>(pAutowirer, p, q)
  {}

  virtual cpp11::shared_ptr<Object> AsObject() {
    return cpp11::static_pointer_cast<Object, T>(*this);
  }
};

