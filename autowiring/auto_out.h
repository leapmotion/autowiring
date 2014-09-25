// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include MEMORY_HEADER
#include <autowiring/AutoPacket.h>

/// <summary>
/// Declares an output of an AutoFilter method via an argument.
/// </summary>
/// <remarks>
/// Declaring an output argument does not guarantee its presence.
/// However, any attempt to access the referenced data will trigger
/// instantiation and a commitment to output the data.
/// Output via the default constructor can be forced by calling
///  auto_out<type>::make().
/// Shared construction and assignment require a commitment to output
/// in order to avoid the possibility of multiple outputs.
/// Moved construction and assignment removes the option to output
/// from the r-value operand, but does *not* force an output.
/// </remarks>
template <class type>
class auto_out:
  protected std::shared_ptr<type>
{
protected:
  std::shared_ptr<AutoPacket> m_packet;
  const std::type_info* m_source;

public:
  typedef type id_type;
  typedef type& base_type;
  typedef std::shared_ptr<type> shared_type;

  static const bool is_input = false;
  static const bool is_output = true;

  operator base_type () {
    make();
    return *(this->get());
  }

  operator shared_type () {
    make();
    return *this;
  }

  auto_out ():
    m_source(nullptr)
  {}

  /// <summary>Share ownership of rhs</summary>
  auto_out (auto_out<type>& rhs) {
    *this = rhs;
  }

  /// <summary>Move ownsership of rhs</summary>
  auto_out (auto_out<type>&& rhs) {
    *this = std::move(rhs);
  }

  /// <summary>Share ownership of rhs</summary>
  /// <remarks>
  /// Any attempt to share ownership requires a
  /// commitment to output, otherwise there could
  /// be multiple outputs.
  /// </remarks>
  auto_out& operator = (auto_out<type>& rhs) {
    reset();
    rhs.make();
    shared_type::operator = (rhs);
    m_packet = rhs.m_packet;
    m_source = rhs.m_source;
    return *this;
  }

  /// <summary>Move ownership rhs to this</summary>
  /// <remarks>
  /// This will remove the AutoPacket reference from
  /// rhs, so no output conflict can arise.
  /// However, this does not force a commitment to output.
  /// </remarks>
  auto_out& operator = (auto_out<type>&& rhs) {
    reset();
    swap(rhs);
    return *this;
  }

  auto_out (std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    m_packet(packet),
    m_source(&source)
  {}

  /// <returns>True when commited to output</returns>
  operator bool () const {
    return shared_type::operator bool();
  }

  /// <summary>Exchanges this and rhs</summary>
  void swap(auto_out<type>& rhs) {
    std::swap<shared_type>(*this, rhs);
    std::swap<std::shared_ptr<AutoPacket>>(m_packet, rhs.m_packet);
    std::swap<const std::type_info*>(m_source, rhs.m_source);
  }

  /// <summary>Finalizes output and prevents subsequent output</summary>
  void reset() {
    shared_type::reset();
    m_packet.reset();
    m_source = nullptr;
  }

  /// <summary>Commit to output of templated type</summary>
  /// </remarks>
  /// This will be called when any attempt is made to access
  /// the referenced type via the auto_out methods get(), *, ->.
  /// <remarks>
  void make(void) {
    if (!m_packet || !m_source)
      return;
    std::shared_ptr<AutoPacket> packet(m_packet);
    const std::type_info* source(m_source);
    shared_type::reset(new type(), [packet, source](type* ptr) {
      packet->Put(ptr, *source);
    });
    // Prevent any subsequent commitments to output
    m_packet.reset();
    m_source = nullptr;
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
