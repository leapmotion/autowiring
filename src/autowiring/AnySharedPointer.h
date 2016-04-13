// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "fast_pointer_cast.h"

/// AnySharedPointer
struct AnySharedPointer {
public:
  AnySharedPointer(void) = default;
  AnySharedPointer(AnySharedPointer&& rhs);
  AnySharedPointer(const AnySharedPointer& rhs) = default;
  AnySharedPointer(std::nullptr_t) :
    AnySharedPointer()
  {}

  template<class T>
  AnySharedPointer(const std::shared_ptr<T>& rhs) :
    m_ti(auto_id_t<T>{}),
    m_ptr(rhs)
  {}

  ~AnySharedPointer(void);

protected:
  auto_id m_ti;
  std::shared_ptr<void> m_ptr;

public:
  explicit operator bool(void) const { return (bool)m_ptr; }
  void* ptr(void) const { return m_ptr.get(); }
  bool empty(void) const { return !m_ptr; }
  auto_id type(void) const { return m_ti; }

  std::shared_ptr<void>& operator*(void) { return m_ptr; }
  const std::shared_ptr<void>& operator*(void) const { return m_ptr; }

  void reset(void) {
    m_ptr.reset();
  }

  std::shared_ptr<CoreObject> as_obj(void) const {
    return
      m_ti.block->pToObj ?
      m_ti.block->pToObj(m_ptr) :
      nullptr;
  }

  /// <summary>
  /// Attempts to dynamically assign this slot to the specified object without changing the current type
  /// </summary>
  /// <returns>True if the assignment succeeds</returns>
  bool try_assign(const std::shared_ptr<CoreObject>& rhs) {
    if (!m_ti.block->pFromObj)
      return false;
    auto ptr = m_ti.block->pFromObj(rhs);
    if (!ptr)
      return false;
    m_ptr = std::move(ptr);
    return true;
  }

  /// <summary>
  /// Attempts to dynamically assign this slot to the specified object without changing the current type
  /// </summary>
  /// <returns>True if the assignment succeeds</returns>
  bool try_assign(const AnySharedPointer& rhs) {
    auto obj = rhs.as_obj();
    return obj && try_assign(obj);
  }

  template<class T>
  const std::shared_ptr<T>& as(void) const {
    // The safety of this routine is verified by the AnySharedPointer unit tests
    return *reinterpret_cast<const std::shared_ptr<T>*>(&m_ptr);
  }

  template<class T>
  std::shared_ptr<T>& as(void) {
    return *reinterpret_cast<std::shared_ptr<T>*>(&m_ptr);
  }

  bool operator==(const AnySharedPointer& rhs) const {
    // Need to compare the control blocks, not the pointer values, because we could be pointing to
    // different spots in the same object.
    return
      !m_ptr.owner_before(rhs.m_ptr) &&
      !rhs.m_ptr.owner_before(m_ptr);
  }

  template<typename T>
  bool operator==(const std::shared_ptr<T>& rhs) const {
    return m_ptr == rhs;
  }

  bool operator!=(std::nullptr_t) const {
    return !!m_ptr;
  }

  template<typename T>
  void init(void) {
    m_ti = auto_id_t<T>{};
    m_ptr.reset();
  }

  // Additional operator overloads:
  bool operator<(const AnySharedPointer& rhs) const { return m_ptr < rhs.m_ptr;}
  bool operator!=(const AnySharedPointer& rhs) const { return !(*this == rhs); }

  void operator=(AnySharedPointer&& rhs) {
    m_ti = rhs.m_ti;
    m_ptr = std::move(rhs.m_ptr);
    rhs.m_ptr.reset();
  }

  void operator=(const AnySharedPointer& rhs) {
    m_ti = rhs.m_ti;
    m_ptr = rhs.m_ptr;
  }

  // Allows dynamic assignment of the type directly from an auto_id field
  void operator=(auto_id ti) {
    m_ti = ti;
    m_ptr.reset();
  }

  /// <summary>
  /// Convenience overload for shared pointer assignment
  /// </summary>
  template<class T>
  void operator=(const std::shared_ptr<T>& rhs) {
    m_ptr = rhs;
  }
};

/// <summary>
/// Convenience implementation of AnySharedPointer which is initially of type T
/// </summary>
/// <remarks>
/// Using this type will automatically ensure that the underlying auto_id is fully instantiated
/// </remarks>
template<class T>
class AnySharedPointerT:
  public AnySharedPointer
{
public:
  AnySharedPointerT(void):
    AnySharedPointer(std::shared_ptr<T>{})
  {}

  AnySharedPointerT(T&& value) :
    AnySharedPointer(std::make_shared<T>(std::forward<T&&>(value)))
  {}

  template<typename U>
  AnySharedPointerT(U&& value) :
    AnySharedPointer(std::make_shared<T>(std::forward<U&&>(value)))
  {}

  T& operator*(void) { return *as<T>(); }
  const T& operator*(void) const { return *as<T>(); }

  T* operator->(void) { return as<T>()->get(); }
  const T* operator->(void) const { return as<T>()->get(); }

  const std::shared_ptr<T>& get(void) const {
    return AnySharedPointer::as<T>();
  }
};

template<class T>
inline bool operator==(const std::shared_ptr<T>& lhs, const AnySharedPointer& rhs) {
  return rhs == lhs;
}
