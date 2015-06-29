// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

#include "AutoPacket.h"
#include "autowiring_error.h"

/// <summary>
/// The auto_out<T> class provides a way to implement deferred-output autofilters.
/// </summary>
/// <remarks>
/// An `auto_out<T>` parameter in an autofilter is an output parameter equivalent to type T.  It stores a
/// std::shared_ptr to the AutoPacket from which the autofilter call was generated, and essentially just
/// waits to be assigned to, at which point it decorates the packet with the assigned value.  If the auto_out<T>
/// instance (and all copies of it) is destroyed without being assigned to, then it marks T as unsatisfiable,
/// which has particular consequences (see AutoPacket::MarkUnsatisfiable).
///
/// In particular, auto_out<T> allows one to define asynchronously processed autofilters by deferring output
/// decoration until much later than the filter itself was called.  For example, one could store the auto_out<T>,
/// begin an asynchronous process on a different thread, and assign to the auto_out<T> once that process completes.
///
/// The auto_out<T> class has shared_ptr style semantics (pointer dereferencing), and additionally can be assigned
/// to from various types, primary including T and its relatives (e.g. `const T&` and `T&&` and `std::shared_ptr<T>`.
/// When assigned to from a T-like typed value, the packet is decorated with that value.  Assignment can only happen
/// once.
///
/// Note that `auto_out<T>` as an output parameter of an autofilter is conceptually less strict than `T&`.  The former
/// does not guarantee that T will be decorated onto the packet by the time the autofilter method returns, and the
/// determination of which may require reasoning about the run-time state of the program.  The latter guarantees that
/// T will be decorated onto the packet by the time the autofilter method returns.  Use `T&` if possible, and only
/// use auto_out<T> when the deferred output behavior is required.
/// </remarks>
template <class T>
class auto_out
{
public:

  // Constructs an empty auto_out object.
  auto_out () { }
  // This should only be called by Autowiring internals.
  auto_out (AutoPacket &packet) : m_auto_out_impl(std::make_shared<auto_out_impl>(packet)) { }
  
  auto_out (const auto_out &ao) : m_auto_out_impl(ao.m_auto_out_impl) { }
  auto_out (auto_out &&ao) : m_auto_out_impl(std::move(ao.m_auto_out_impl)) { }
  auto_out &operator= (const auto_out &ao) { m_auto_out_impl = ao.m_auto_out_impl; }
  auto_out &operator= (auto_out &&ao) { m_auto_out_impl = std::move(ao.m_auto_out_impl); }

  // Returns true iff this auto_out object is nonempty (i.e. refers to a packet).
  operator bool () const { return bool(m_auto_out_impl); }
  // Returns a const reference to the decorated value, if this auto_out object has been assigned to, otherwise throws.
  const T &operator* () const { ThrowIfInvalid(); return m_auto_out_impl->operator*(); }
  // Returns a const pointer to the decorated value, if this auto_out object has been assigned to, otherwise throws.
  const T *operator-> () const { ThrowIfInvalid(); return m_auto_out_impl->operator->(); }
  // Assign to this auto_out object from a T value (via const reference).
  auto_out &operator= (const T &t) { ThrowIfInvalid(); m_auto_out_impl->operator=(t); }
  // Assign to this auto_out object from a T value (via r-value reference)
  auto_out &operator= (T &&t) { ThrowIfInvalid(); m_auto_out_impl->operator=(std::forward<T>(t)); }
  // Assign to this auto_out object from a T value (via shared_ptr).
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
    const T &operator* () const {
      AssertValidity();
      if (!m_decoration)
        throw autowiring_error("Can't get a reference to an instance of T before it has been Decorated to the packet (in this case by assigning to the auto_out<T> via operator=.");
      return *m_decoration;
    }
    const T *operator-> () const {
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
