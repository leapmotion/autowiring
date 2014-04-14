#pragma once
#include "Autowired.h"
#include "ObjectPool.h"

class EnclosureBase
{
public:
  virtual ~EnclosureBase(void) {}

  // Causes this object to be released and memory potentially reclaimed, where possible
  // The corresponding slot will appear empty, and any attempts to extract a pointer will
  // return null in the event of a released field.
  virtual void Release(void) = 0;

  // Causes the held object by this enclosure to be reinitialized
  virtual void Reset(void) = 0;
};

template<class T>
class Enclosure:
  public EnclosureBase
{
public:
  Enclosure(void) {}
  ~Enclosure(void) {}

  virtual void operator=(T&& rhs) = 0;
  virtual T* get(void) = 0;
};

/// <summary>
/// A type enclosure, used to provide support similar to boost::any
/// </summary>
/// <remarks>
/// We use this instead of boost::any so we can have access to a rvalue-move ctor.
/// </remarks>
template<class T>
class EnclosureImpl:
  public Enclosure<T>
{
public:
  EnclosureImpl(void) :
    m_isInitialized(false)
  {}

  EnclosureImpl(const T& held) :
    m_isInitialized(false),
    m_held(held)
  {}

  EnclosureImpl(T&& held) :
    m_isInitialized(true)
  {
    new (m_held) T(std::move(held));
  }

  ~EnclosureImpl(void) {
    if(m_isInitialized)
      ((T*) m_held)->~T();
  }

private:
  bool m_isInitialized;
  unsigned char m_held[sizeof(T)];

public:
  // Causes this object to be released and memory potentially reclaimed, where possible
  virtual void Release(void) override {
    if(m_isInitialized)
      ((T*) m_held)->~T();
    m_isInitialized = false;
  }

  // Causes the held object by this enclosure to be reinitialized
  virtual void Reset(void) override {
    if(!m_isInitialized)
      new (m_held) T();
    m_isInitialized = true;
  }

  void operator=(T&& rhs) override {
    if(m_isInitialized)
      (T&) *this = std::move(rhs);
    else {
      m_isInitialized = true;
      new (m_held) T(std::move(rhs));
    }
  }

  T* get(void) override { return m_isInitialized ? (T*) m_held : nullptr; }
};

template<class T>
class EnclosureImpl<auto_pooled<T>>:
  public Enclosure<auto_pooled<T>>
{
public:
  EnclosureImpl(void) {}

  EnclosureImpl(T&& rhs) {
    (*m_pool)(m_ptr);
    *m_ptr = std::move(rhs);
  }

  // Pool and corresponding shared pointer:
  AutoRequired<ObjectPool<T>> m_pool;
  auto_pooled<T> m_ptr;

  void Release(void) override {
    m_ptr.reset();
  }

  void Reset(void) override {
    if(!m_ptr)
      (*m_pool)(m_ptr);
  }

  void operator=(auto_pooled<T>&& rhs) override {
    m_ptr = std::move(rhs);
  }

  void operator=(T&& rhs) {
    if(!m_ptr)
      (*m_pool)(m_ptr);
    *m_ptr = std::move(rhs);
  }

  auto_pooled<T>* get(void) override { return &m_ptr; }
};
