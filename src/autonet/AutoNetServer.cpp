// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutoNetServer.h"
#include "at_exit.h"
#include <iostream>

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer"),
  m_Server(std::make_shared<server>()),
  m_Port(8000)
{
  m_Server->set_message_handler([this] (websocketpp::connection_hdl hdl, message_ptr msg) {
    OnMessage(hdl, msg);
  });

  for (auto type = g_pFirstEntry; type; type = type->pFlink) {
    if (type->IsEventReceiver())
      m_EventTypes.insert(type->NewTypeIdentifier());
    
    if (type->CanInject()){
      m_AllTypes[type->ti.name()] = [type]{
        std::cout
        << "Injecting "
        << type->ti.name()
        << " into "
        << AutoCurrentContext()->GetSigilType().name()
        << std::endl;
        type->Inject();
      };
    }
  }
}

AutoNetServer::~AutoNetServer(){}

//Core Thread methods
void AutoNetServer::Run(void){
  std::cout << "Starting Autonet server..." << std::endl;

  m_Server->init_asio();
  m_Server->start_accept();
  m_Server->run();
  
  PollThreadUtilization(std::chrono::milliseconds(1000));
  CoreThread::Run();
}

void AutoNetServer::OnStop(void){
  for (auto sub : m_Subscribers){
    SendMessage(sub, "Closing");
  }
  m_Server->stop();
}

void AutoNetServer::OnMessage(websocketpp::connection_hdl hdl, message_ptr p_message) {
  Jzon::Object msg;
  Jzon::Parser parser(msg, p_message->get_payload());
  if(!parser.Parse()) {
    std::cout << "Couldn't parse message" << std::endl;
    SendMessage(hdl, "invalidMessage", "Couldn't parse message");
    return;
  }

  std::string msgType = msg.Get("type").ToString();
  Jzon::Array msgArgs = msg.Get("args");

  *this += [this, hdl, msgType, msgArgs] {
    if(msgType == "subscribe") HandleSubscribe(hdl);
    else if(msgType == "unsubscribe") HandleUnsubscribe(hdl);
    else if(msgType == "terminateContext") HandleTerminateContext(msgArgs.Get(0).ToInt());
    else if(msgType == "injectContextMember") HandleInjectContextMember(msgArgs.Get(0).ToInt(), msgArgs.Get(1).ToString());
    else if(msgType == "resumeFromBreakpoint") HandleResumeFromBreakpoint(msgArgs.Get(0).ToString());
    else
      SendMessage(hdl, "invalidMessage", "Message type not recognized");
  };
}

void AutoNetServer::Breakpoint(std::string name){
  std::unique_lock<std::mutex> lk(m_mutex);

  m_breakpoints.insert(name);
  
  *this += [this, name]{
    BroadcastMessage("breakpoint", name);
  };
  
  m_breakpoint_cv.wait(lk, [this, name]{
    return !m_breakpoints.count(name);
  });
}

// Update Functions
void AutoNetServer::NewContext(CoreContext& newCtxt){
  auto ctxt = newCtxt.shared_from_this();

  *this += [this, ctxt] {
    Jzon::Object context;
    context.Add("name", ctxt->GetSigilType().name());

    if (ctxt->GetParentContext()){
      context.Add("parent", ResolveContextID(ctxt->GetParentContext().get()));
    }

    BroadcastMessage("newContext", ResolveContextID(ctxt.get()), context);
  };
}

void AutoNetServer::ExpiredContext(CoreContext& oldCtxt){
  int id = ResolveContextID(&oldCtxt);
  *this += [this, id] {
    BroadcastMessage("expiredContext", id);
  };
}

void AutoNetServer::NewObject(CoreContext& ctxt, const AnySharedPointer& object){
  int contextID = ResolveContextID(&ctxt);
  
  *this += [this, object, contextID]{
    Jzon::Object objData;
    Jzon::Object types;
    std::shared_ptr<Object> objectPtr = *object;

    // Add object data
    objData.Add("name", typeid(*objectPtr).name());
    {
      Jzon::Array slots;
      for (auto slot = object->GetSlotInformation().pHead; slot; slot = slot->pFlink) {
        Jzon::Object slotData;
        slotData.Add("name", slot->type.name());
        slotData.Add("autoRequired", slot->autoRequired);
        slotData.Add("offset", int(slot->slotOffset));
        slots.Add(slotData);
      }
      objData.Add("slots", slots);
    }
    
    // Add type information
    auto member = leap::fast_pointer_cast<ContextMember>(objectPtr);
    if (member) {
      types.Add("contextMember", true);
    }

    auto runnable = leap::fast_pointer_cast<CoreRunnable>(objectPtr);
    if (runnable) {
      types.Add("coreRunnable", true);
    }
    
    auto thread = leap::fast_pointer_cast<BasicThread>(objectPtr);
    if(thread) {
      m_Threads[thread->GetSelf<BasicThread>()];

      Jzon::Object utilization;
      utilization.Add("kernel", 0.0);
      utilization.Add("user", 0.0);
      types.Add("thread", utilization);
    }

    auto eventRcvr = leap::fast_pointer_cast<EventReceiver>(objectPtr);
    if (eventRcvr) {
      Jzon::Array listenerTypes;
      for (auto event : m_EventTypes) {
        if (event->Is(objectPtr.get()))
          listenerTypes.Add(event->Type().name());
      }

      types.Add("eventReceiver", listenerTypes);
    }

    auto filter = leap::fast_pointer_cast<ExceptionFilter>(objectPtr);
    if (filter) {
      types.Add("exceptionFilter", true);
    }

    auto bolt = leap::fast_pointer_cast<BoltBase>(objectPtr);
    if (bolt) {
      Jzon::Array sigils;
      for(auto cur = bolt->GetContextSigils(); *cur; cur++){
        sigils.Add((*cur)->name());
      }
      types.Add("bolt", sigils);
    }
  
    BroadcastMessage("newObject", contextID, types, objData);
  };
  
}

void AutoNetServer::EventFired(CoreContext& context, const std::type_info& info){
  int contextID = ResolveContextID(&context);
  std::string name = info.name();

  *this += [this, contextID, name] {
    Jzon::Object eventHash;
    eventHash.Add("name", name);

    BroadcastMessage("eventFired", contextID, eventHash);
  };
}

void AutoNetServer::SendMessage(websocketpp::connection_hdl hdl, const char* p_type) {
  Jzon::Object msg;
  msg.Add("type", p_type);
  Jzon::Array arguments;
  msg.Add("args", arguments);
  SendMessage(hdl, msg);
}

void AutoNetServer::SendMessage(websocketpp::connection_hdl hdl, const Jzon::Object& msg) {
  Jzon::Writer writer(msg, Jzon::NoFormat);
  writer.Write();
  m_Server->send(hdl, writer.GetResult(), websocketpp::frame::opcode::text);
}

void AutoNetServer::HandleTerminateContext(int contextID) {
  ResolveContextID(contextID)->SignalShutdown();
}

void AutoNetServer::HandleInjectContextMember(int contextID, std::string typeName) {
  std::shared_ptr<CoreContext> ctxt = ResolveContextID(contextID)->shared_from_this();
  
  if (m_AllTypes.find(typeName) != m_AllTypes.end()) {
    CurrentContextPusher pshr(ctxt);
    m_AllTypes[typeName]();
  } else {
    // Type doesn't exist
    assert(false);
  }
}

void AutoNetServer::HandleResumeFromBreakpoint(std::string name){
  std::unique_lock<std::mutex> lk(m_mutex);
  
  m_breakpoints.erase(name);
  m_breakpoint_cv.notify_all();
}

//helper functions
int AutoNetServer::ResolveContextID(CoreContext* ctxt) {
  static int counter = 0;

  if ( m_ContextIDs.find(ctxt) == m_ContextIDs.end() ){
    m_ContextIDs[ctxt] = counter;
    m_ContextPtrs[counter] = ctxt;
    return counter++;
  } else {
    return m_ContextIDs[ctxt];
  }
}

CoreContext* AutoNetServer::ResolveContextID(int id) {
  return m_ContextPtrs.at(id);
}

void AutoNetServer::PollThreadUtilization(std::chrono::milliseconds period){

  *this += period, [this, period] {
    
    for(auto q = m_Threads.begin(); q != m_Threads.end();) {
      std::shared_ptr<BasicThread> thread = q->first.lock();
      if(!thread) {
        m_Threads.erase(q++);
        continue;
      }
      
      std::chrono::milliseconds runtimeKM, runtimeUM;
      thread->GetThreadTimes(runtimeKM, runtimeUM);

      // Determine the amount of time this thread has run since the last time we
      // asked it for its runtime.
      std::chrono::duration<double> deltaRuntimeKM = runtimeKM - q->second.m_lastRuntimeKM;
      std::chrono::duration<double> deltaRuntimeUM = runtimeUM - q->second.m_lastRuntimeUM;

      // Update timing values:
      q->second.m_lastRuntimeKM = runtimeKM;
      q->second.m_lastRuntimeUM = runtimeUM;

      // Broadcast current thread utilization
      int contextID = ResolveContextID(thread->GetContext().get());
      std::string name = typeid(*thread.get()).name();

      std::chrono::duration<double> periodDbl = period;
      double kmPercent = 100.0 * (deltaRuntimeKM.count() / periodDbl.count());
      double umPercent = 100.0 * (deltaRuntimeUM.count() / periodDbl.count());
      
      // Make sure user + kernel percent < 100.0
      umPercent = std::min(umPercent, 99.9 - kmPercent);
      
      if (kmPercent >= 0.0 && umPercent >= 0.0) {
        BroadcastMessage("threadUtilization", contextID, name, kmPercent, umPercent);
      }

      // Next!
      q++;
    }
    
    // Poll again after "period" milliseconds
    PollThreadUtilization(period);
  };
}
