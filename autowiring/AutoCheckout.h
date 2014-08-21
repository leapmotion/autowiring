// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

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
    rhs.m_val = nullptr;
  }

  ~AutoCheckout(void) {
    if(m_val)
      (m_parent->*completion)(m_ready, *m_source);
  }

private:
  AutoPacket* m_parent;
  std::shared_ptr<T> m_val;
  bool m_ready;
  const std::type_info* m_source;
  t_completion completion;

public:
  /// <summary>
  /// Causes the wrapped packet to be committed when the checkout is destroyed
  /// </summary>
  /// <param="source">attributes data source to specified type</param>
  void Ready(const std::type_info& source = typeid(void)) {
    m_ready = true;
    m_source = &source;
  }

  // Operator overloads:
  operator T*(void) const { return m_val.get(); }
  T& operator*(void) const { return *m_val; }
  T* operator->(void) const { return m_val.get(); }
  operator bool(void) const { return !!m_val; }
  operator T&(void) const { return *m_val; }

  AutoCheckout& operator=(AutoCheckout&& rhs) {
    std::swap(m_parent, rhs.m_parent);
    std::swap(m_val, rhs.m_val);
    m_ready = rhs.m_ready;
    m_source = rhs.m_source;
    completion = rhs.completion;
    return *this;
  }
};
