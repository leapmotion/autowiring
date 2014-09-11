// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/is_autofilter.h>
#include MEMORY_HEADER

/// <summary>
/// AutoStile provides a means of calling a context as though it is an AutoFilter.
/// The AutoStile Args use the same syntax as AutoFilter:
/// - "const T&" denote inputs to the slave context.
/// - "auto_out<T>" denote an outputs from the slave context.
/// PROBLEM: shared_ptr references cannot be used, so data is always copied.
/// PROBLEM: T& inputs cannot be used, since they will be destroyed before their
/// WARNING: auto_out<T> will *still* be conditional on the production of that type
/// in the slave context.
/// value can be set.
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
  typename std::enable_if<is_autofilter_arg<data_pipe>::is_input, bool>::type DecorateContext(std::shared_ptr<AutoPacket>& slave_packet, std::shared_ptr<AutoPacket>& master_packet, data_pipe master_data) {
    slave_packet->Decorate(master_data);
    return true;
  }

  /// <summar>Decorator for output creates an extraction for slave_data</summary>
  template<class data_pipe>
  typename std::enable_if<is_autofilter_arg<data_pipe>::is_output, bool>::type DecorateContext(std::shared_ptr<AutoPacket>& slave_packet, std::shared_ptr<AutoPacket>& master_packet, data_pipe master_data) {
    static_assert(is_auto_out<data_pipe>::value, "AutoStile outputs must be auto_out<T>");
    // Extract the base type and define the input type "const base_type&"
    typedef typename is_autofilter_arg<data_pipe>::type base_type;
    typedef typename std::add_lvalue_reference<
      typename std::add_const<
        base_type
      >::type
    >::type in_type;

    slave_packet->AddRecipient<void, in_type>(std::function<void(in_type slave_data)>([master_data](in_type slave_data) {
      //NOTE: The lambda copy of master_data is implicitly a move of AutoCheckout,
      // so this lambda has sole responsability for calling CompleteCheckout.
      *master_data = slave_data;
      master_data.Ready();
    }));
    return true;
  }

public:
  AutoStile(std::weak_ptr<CoreContext> slave_context = std::weak_ptr<CoreContext>()) {
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

  void Leash(std::weak_ptr<CoreContext> slave_context = std::weak_ptr<CoreContext>()) {
    m_slave_factory.reset();

    std::shared_ptr<CoreContext> strong_context = slave_context.lock();
    if (!strong_context)
      return;

    strong_context->NotifyWhenAutowired<AutoPacketFactory>([this, slave_context](){
      // NOTE: Call-back is not necessarily inside the notifying context.
      std::shared_ptr<CoreContext> strong_context = slave_context.lock();
      if (!strong_context)
        return;
      std::shared_ptr<AutoPacketFactory> slave_factory;
      strong_context->FindByTypeRecursive(slave_factory);
      m_slave_factory = slave_factory;
    });
  }
};
