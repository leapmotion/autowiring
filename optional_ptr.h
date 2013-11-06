#pragma once

/// <summary>
/// A wrapper type, used to indicate a pointer which cannot be deferred
/// </summary>
/// <remarks>
/// An optional pointer denotes a field which may be omitted during a call to a filter
/// method.  The caller is responsible for testing a field for correctness.
///
/// Consumers are cautioned against using optional_ptr where not necessary.  Users of
/// optional_ptr will only be notified that a field is satisfied until the packet is
/// about to be destroyed, or the field has been explicitly marked as unsatisfiable
/// (for example, due to a failed checkout).
/// </remarks>
template<class T>
class optional_ptr
{
public:
  optional_ptr(const T* ptr = nullptr) :
    m_ptr(ptr)
  {}

  optional_ptr(const T& rhs) :
    m_ptr(&rhs)
  {}

  optional_ptr(const optional_ptr& rhs) :
    m_ptr(rhs.m_ptr)
  {}

private:
  // This is the only member of this class.  Because of how it's used, the optional_ptr
  // class MUST NOT be resized by the introduction of other members.
  const T* const m_ptr;

public:
  operator bool(void) const { return !!m_ptr; }
  const T* operator->(void) const { return m_ptr; }
};

