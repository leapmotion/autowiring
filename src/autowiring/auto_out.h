// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"
#include "autowiring_error.h"
#include MEMORY_HEADER

class AutoPacket;

namespace autowiring {

template<class T>
class auto_arg;

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
template<class T>
class auto_out
{
public:
  auto_out(void) = default;
  auto_out(const auto_out& ao) = default;
  auto_out(AutoPacket& packet) :
    m_auto_out_impl(std::make_shared<auto_out_impl>(packet))
  {}

  auto_out(auto_out&& rhs) :
    m_auto_out_impl(std::move(rhs.m_auto_out_impl))
  {}

  struct auto_out_impl {
  public:
    auto_out_impl(void) = delete;
    auto_out_impl(const auto_out_impl &aoi) = default;

    auto_out_impl(auto_out_impl&& rhs) :
      m_packet(std::move(rhs.m_packet)),
      m_decoration(std::move(rhs.m_decoration))
    {}

    auto_out_impl(AutoPacket& packet) :
      m_packet(packet.shared_from_this())
    {}

    ~auto_out_impl(void) {
      if (m_decoration)
        m_packet->Decorate<T>(std::move(m_decoration));
      else
        m_packet->MarkUnsatisfiable<T>();
    }

  private:
    std::shared_ptr<AutoPacket> m_packet;
    std::shared_ptr<T> m_decoration;

  public:
    T &operator*(void) const {
      return *m_decoration;
    }
    T *operator->(void) const {
      return m_decoration.get();
    }
    void operator=(const T& t) {
      m_decoration = std::make_shared<T>(t);
    }
    void operator=(T&& t) {
      m_decoration = std::make_shared<T>(std::forward<T&&>(t));
    }
    void operator=(const std::shared_ptr<T>& t) {
      m_decoration = t;
    }
  };

private:
  std::shared_ptr<auto_out_impl> m_auto_out_impl;

public:
  // Returns true iff this auto_out object is nonempty (i.e. refers to a packet).
  explicit operator bool(void) const { return bool(m_auto_out_impl); }

  /// <summary>
  /// Releases the internal implementatoin object
  /// </summary>
  /// <remarks>
  /// If this auto_out is the last handle to this output instance, this method will cause
  /// the attached decoration to be assigned
  /// </remarks>
  void reset(void) {
    m_auto_out_impl.reset();
  }

  auto_out_impl& operator*(void) const { return *m_auto_out_impl; }
  void operator=(auto_out&& rhs) { m_auto_out_impl = std::move(rhs.m_auto_out_impl); }
};

template<class T>
class auto_arg<auto_out<T>>
{
public:

  typedef auto_out<T> type;
  typedef auto_out<T> arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static auto_out<T> arg(AutoPacket& packet) {
    return auto_out<T>(packet);
  }

  static void Commit(AutoPacket&, const type&) {
    // Do nothing -- auto_out does its own deferred decoration.
  }
};

}
