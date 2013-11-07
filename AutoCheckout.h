#pragma once
#include <utility>

class AutoPacket;

template<class T>
class AutoCheckout {
public:
  AutoCheckout(void) :
    m_parent(nullptr),
    m_val(nullptr),
    m_ready(false)
  {}

  AutoCheckout(AutoPacket& parent, T* val) :
    m_parent(&parent),
    m_val(val),
    m_ready(false)
  {}

  AutoCheckout(AutoCheckout&& rhs) :
    m_parent(rhs.m_parent),
    m_val(rhs.m_val),
    m_ready(rhs.m_ready)
  {
    rhs.m_parent = nullptr;
    rhs.m_val = nullptr;
  }

  ~AutoCheckout(void) {
    if(m_val)
      m_parent->CompleteCheckout<T>(m_ready);
  }

private:
  AutoPacket* m_parent;
  T* m_val;
  bool m_ready;

public:
  /// <summary>
  /// Causes the wrapped packet to be committed when the checkout is destroyed
  /// </summary>
  void Ready(void) {
    m_ready = true;
  }

  // Operator overloads:
  operator T*(void) const { return m_val; }
  T* operator->(void) const { return m_val; }
  operator bool(void) const { return !!m_val; }
  operator T&(void) const { return *m_val; }

  AutoCheckout& operator=(AutoCheckout&& rhs) {
    std::swap(m_parent, rhs.m_parent);
    std::swap(m_val, rhs.m_val);
    return *this;
  }
};