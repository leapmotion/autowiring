// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <typeinfo>
#include MEMORY_HEADER

class AutoPacket;

template<class T>
class AutoCheckout {
public:
  typedef void (AutoPacket::*t_completion)(bool, const std::type_info&);

  AutoCheckout(void) :
    m_parent(nullptr),
    m_val(nullptr),
    m_ready(false),
    m_source(&typeid(void)),
    completion(nullptr)
  {}

  AutoCheckout(AutoPacket& parent, const std::shared_ptr<T>& val, t_completion completion, const std::type_info& source = typeid(void)) :
    m_parent(&parent),
    m_val(val),
    m_ready(false),
    m_source(&source),
    completion(completion)
  {}

  AutoCheckout(AutoCheckout&& rhs) :
    m_parent(rhs.m_parent),
    m_val(rhs.m_val),
    m_ready(rhs.m_ready),
    m_source(rhs.m_source),
    completion(rhs.completion)
  {
    rhs.m_parent = nullptr;
    rhs.m_val.reset();
    rhs.m_ready = false;
  }

  ~AutoCheckout(void) {
    if(m_val)
      (m_parent->*completion)(m_ready, *m_source);
  }

private:
  // NOTE: m_parent cannot be a shared_ptr since it may be created during the resolution
  // of optional_ptr arguments, in which case the shared_ptr to m_parent will be expired.
  // Using a regular pointer enables decoration during the final call.
  AutoPacket* m_parent;
  std::shared_ptr<T> m_val;
  mutable bool m_ready;
  const std::type_info* m_source;
  t_completion completion;

public:
  /// <summary>
  /// Causes the wrapped packet to be committed when the checkout is destroyed
  /// </summary>
  void Ready() const {
    m_ready = true;
  }

  // Operator overloads:
  operator T*(void) const { return m_val.get(); }
  T& operator*(void) const { return *m_val; }
  T* operator->(void) const { return m_val.get(); }
  operator bool(void) const { return !!m_val; }
  operator T&(void) const { return *m_val; }

  // <summary>Assignment by move</summary>
  AutoCheckout& operator=(AutoCheckout&& rhs) {
    m_parent = rhs.m_parent;
    m_val = rhs.m_val;
    m_ready = rhs.m_ready;
    m_source = rhs.m_source;
    completion = rhs.completion;

    rhs.m_parent = nullptr;
    rhs.m_val.reset();
    rhs.m_ready = false;

    return *this;
  }
};
