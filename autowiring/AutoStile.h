// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/is_autofilter.h>
#include MEMORY_HEADER

/// <summary>
/// AutoStile provides a means of calling a context as though it is an AutoFilter.
/// The AutoStile Args use the same syntax as AutoFilter:
/// - "const T&" denote inputs to the slave context.
/// - "T&" denote an outputs from the slave context.
/// TODO: Wrapper classes (auto_out, optional_ptr) cannot be used.
/// PROBLEM: shared_ptr references cannot be used, so data is always copied.
/// </summary>
/// <remarks>
/// When AutoStile::AutoFilter is called it initiates execution of a Slave context,
/// and injects the objects referenced by AutoFilter into the new context.
/// AutoStile also handles the extraction of data requested from the context.
///
/// If merged data is required from the context then it is necessary to decorate the
/// master_packet with an extraction function, and declare the type of that function
/// as an *input* argument of the AutoStile.
///
/// TODO: Example code to implement this...
///
/// </remarks>
template<class... Args>
class AutoStile
{
  static_assert(all_autofilter_args<Args...>::value, "AutoStile: All template types must be AutoFilter argument types");

protected:
  std::weak_ptr<AutoPacketFactory> m_slave_factory;

  /// <summary>Decorator for input shares the reference to master_data</summary>
  template<class data_pipe>
  typename std::enable_if<is_autofilter_arg<data_pipe>::is_input, bool>::type DecorateContext(std::shared_ptr<AutoPacket>& slave_packet, std::shared_ptr<AutoPacket>& master_packet, data_pipe& master_data) {
    slave_packet->Decorate(master_data);
    return true;
  }

  /// <summar>Decorator for output creates an extraction for slave_data</summary>
  template<class data_pipe>
  typename std::enable_if<is_autofilter_arg<data_pipe>::is_output, bool>::type DecorateContext(std::shared_ptr<AutoPacket>& slave_packet, std::shared_ptr<AutoPacket>& master_packet, data_pipe& master_data) {
    slave_packet->AddRecipient(std::function<void(const data_pipe& slave_data)>([master_packet](const data_pipe& slave_data) {
      master_packet->Decorate(slave_data);
    }));
    return true;
  }

public:
  AutoStile(CoreContext* slave_context = nullptr) {
    Leash(slave_context);
  }

  void AutoFilter(AutoPacket& packet, Args... data) {
    std::shared_ptr<AutoPacketFactory> slave_factory = m_slave_factory.lock();
    if (!slave_factory)
      return;

    // Initiate the slave context
    std::shared_ptr<AutoPacket> master_packet = packet.shared_from_this();
    std::shared_ptr<AutoPacket> slave_packet = slave_factory->NewPacket();
    bool init[] = {DecorateContext<Args>(slave_packet, master_packet, data)...};
    (void)init;
  }

  void Leash(CoreContext* slave_context = nullptr) {
    m_slave_factory.reset();
    if (slave_context) {
      slave_context->NotifyWhenAutowired<AutoPacketFactory>([this,slave_context](){
        // NOTE: Call-back is not necessarily in side the notifying context.
        std::shared_ptr<AutoPacketFactory> slave_factory;
        slave_context->FindByTypeRecursive(slave_factory);
        m_slave_factory = slave_factory;
      });
    }
  }
};
