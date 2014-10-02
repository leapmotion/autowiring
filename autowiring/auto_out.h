// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "AutoPacket.h"
#include MEMORY_HEADER

/// <summary>
/// Declares an output of an AutoFilter method via an argument.
/// </summary>
/// <remarks>
/// Lazy allocation is implemented, and is triggered on destruction.
/// Any attempt to access the referenced data will trigger
/// instantiation and a commitment to output the data.
/// Moved construction and assignment removes the option to output
/// from the r-value operand, but does *not* force an output.
/// </remarks>
template <class type>
class auto_out:
  protected std::shared_ptr<type>
{
  auto_out (auto_out<type>& rhs) = delete;
  auto_out& operator = (auto_out<type>& rhs) = delete;

protected:
  std::shared_ptr<AutoPacket> m_packet;
  const std::type_info* m_source;
  bool m_makeable;

  /// </remarks>
  /// This will be called when any attempt is made to access
  /// the referenced type via the auto_out methods get(), *, ->.
  /// <remarks>
  void make(void) {
    if (!m_makeable)
      return;
    shared_type::reset(new type());
    m_makeable = false;
  }

public:
  typedef type id_type;
  typedef type& base_type;
  typedef std::shared_ptr<type> shared_type;

  static const bool is_input = false;
  static const bool is_output = true;

  operator base_type () {
    make();
    return *get();
  }

  operator shared_type () {
    make();
    return *this;
  }

  /// <summary>Destruction of auto_out makes type data available</summary>
  ~auto_out () {
    if (shared_type::operator bool())
      m_packet->Put(static_cast<shared_type>(*this), *m_source);
    else if (m_makeable)
      m_packet->Put(new type(), *m_source);
  }

  auto_out ():
    m_source(nullptr),
    m_makeable(false)
  {}

  /// <summary>Move ownsership of rhs</summary>
  auto_out (auto_out<type>&& rhs) {
    *this = std::move(rhs);
  }

  /// <summary>Move ownership rhs to this</summary>
  /// <remarks>
  /// This will remove the AutoPacket reference from
  /// rhs, so no output conflict can arise.
  /// However, this does not force a commitment to output.
  /// </remarks>
  auto_out& operator = (auto_out<type>&& rhs) {
    cancel();
    shared_type::operator = (std::move(rhs));
    m_packet = std::move(rhs.m_packet);
    m_source = rhs.m_source;
    rhs.m_source = nullptr;
    m_makeable = rhs.m_makeable;
    rhs.m_makeable = false;
    return *this;
  }

  auto_out (std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    m_packet(packet),
    m_source(&source),
    m_makeable(true)
  {}

  /// <summary>Cancels output and prevents subsequent output</summary>
  void cancel() {
    shared_type::reset();
    m_makeable = false;
  }

  /// <returns>True when commited to output</returns>
  operator bool () const {
    return m_makeable || shared_type::operator bool();
  }

  type* get() {
    make();
    return shared_type::get();
  }

  type& operator* () {
    make();
    return *shared_type::get();
  }
  type* operator-> () {
    make();
    return shared_type::get();
  }

  using shared_type::use_count;
  using shared_type::unique;
  using shared_type::owner_before;
};
