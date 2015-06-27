// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

#include "AutoPacket.h"
#include "autowiring_error.h"

/// <summary>
/// TODO write these comments
/// </summary>
/// <remarks>
/// </remarks>
template <class T>
class auto_out
{
public:

  auto_out () { }
  auto_out (AutoPacket &packet) : m_auto_out_impl(std::make_shared<auto_out_impl>(packet)) { }
  auto_out (const auto_out &ao) : m_auto_out_impl(ao.m_auto_out_impl) { }
  auto_out (auto_out &&ao) : m_auto_out_impl(std::move(ao.m_auto_out_impl)) { }

  operator bool () const { return bool(m_auto_out_impl); }
  const T &operator * () const { ThrowIfInvalid(); return m_auto_out_impl->operator*(); }
  const T *operator -> () const { ThrowIfInvalid(); return m_auto_out_impl->operator->(); }
  auto_out &operator= (const auto_out &ao) { m_auto_out_impl = ao.m_auto_out_impl; }
  auto_out &operator= (auto_out &&ao) { m_auto_out_impl = std::move(ao.m_auto_out_impl); }
  auto_out &operator= (const T &t) { ThrowIfInvalid(); m_auto_out_impl->operator=(t); }
  auto_out &operator= (T &&t) { ThrowIfInvalid(); m_auto_out_impl->operator=(std::forward<T>(t)); }
  auto_out &operator= (const std::shared_ptr<T> &t) { ThrowIfInvalid(); m_auto_out_impl->operator=(t); }
  
private:
  
  void ThrowIfInvalid () const {
    if (!m_auto_out_impl)
      throw autowiring_error("Can't dereference or assign to an invalid auto_out<T>.");
  }
  
  struct auto_out_impl {
  public:
    auto_out_impl () = delete; // This allows us to guarantee that m_packet is always a valid shared_ptr.
    auto_out_impl (AutoPacket &packet) : m_packet(packet.shared_from_this()) { }
    auto_out_impl (const auto_out_impl &aoi) : m_packet(aoi.m_packet), m_decoration(aoi.m_decoration) { }
    auto_out_impl (auto_out_impl &&aoi) : m_packet(std::move(aoi.m_packet)), m_decoration(std::move(aoi.m_decoration)) { }
    ~auto_out_impl () {
      if (!m_decoration)
        m_packet->MarkUnsatisfiable<T>();
    }
    const T &operator * () const {
      AssertValidity();
      if (!m_decoration)
        throw autowiring_error("Can't get a reference to an instance of T before it has been Decorated to the packet (in this case by assigning to the auto_out<T> via operator=.");
      return *m_decoration;
    }
    const T *operator -> () const {
      AssertValidity();
      if (!m_decoration)
        throw autowiring_error("Can't get a reference to an instance of T before it has been Decorated to the packet (in this case by assigning to the auto_out<T> via operator=.");
      return m_decoration.get();
    }
    void operator= (const T &t) {
      AssertValidity();
      if (!m_decoration) {
        m_packet->Decorate<T>(t);
        m_packet->Get<T>(m_decoration);
      }
    }
    void operator= (T &&t) {
      AssertValidity();
      if (!m_decoration) {
        m_packet->Decorate<T>(std::forward<T>(t));
        m_packet->Get<T>(m_decoration);
      }
    }
    void operator= (const std::shared_ptr<T> &t) {
      AssertValidity();
      if (!m_decoration) {
        m_packet->Decorate<T>(t);
        m_packet->Get<T>(m_decoration);
      }
    }
  private:
    void AssertValidity () { assert(bool(m_packet) && "m_packet should never be null."); }
    std::shared_ptr<AutoPacket> m_packet;
    std::shared_ptr<const T> m_decoration;
  };
  
  std::shared_ptr<auto_out_impl> m_auto_out_impl;
};
