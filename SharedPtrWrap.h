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

/// <summary>
/// This class is a generic class intended to wrap a shared pointer
/// </summary>
template<class T, class Mp>
class SharedPtrWrap:
  public SharedPtrWrapImpl<T>
{
public:
  typedef typename Mp::iterator myIter;
  SharedPtrWrap(cpp11::weak_ptr<Autowirer> pAutowirer, T* p, myIter q):
    SharedPtrWrapImpl<T>(pAutowirer, p),
    m_q(q)
  {
  }

  ~SharedPtrWrap(void) {
    cpp11::shared_ptr<Autowirer> autowirer = this->pAutowirer.lock();
    if(autowirer)
      // We don't attempt this if the autowirer is already in teardown, meaning that
      // the map, and all if its members, are already being destroyed or are already
      // gone.
      AutowirerErase(autowirer.get(), m_q);
  }

  myIter m_q;
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
class SharedPtrWrapContext:
  public SharedPtrWrapImpl<T>
{
public:
  typedef typename Mp::iterator myIter;
  SharedPtrWrapContext(cpp11::weak_ptr<Autowirer> pParent, T* p, myIter q):
    SharedPtrWrapImpl<T>(pParent, p)
  {}

  ~SharedPtrWrapContext(void) {
    cpp11::shared_ptr<Autowirer> autowirer = this->pAutowirer.lock();
    if(autowirer)
      AutowirerErase(autowirer.get(), q);
  }

  myIter q;

  virtual cpp11::shared_ptr<Object> AsObject() {
    return
      cpp11::is_base_of<Object, T>::value ?
      cpp11::static_pointer_cast<Object, T>(*this) :
      cpp11::shared_ptr<Object>();
  }
};

