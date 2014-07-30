// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"
#include "at_exit.h"
#include <iostream>
#include FUTURE_HEADER

using std::placeholders::_1;
using std::placeholders::_2;
using json11::Json;

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer"),
  m_Server(std::make_shared<server>()),
  m_Port(8000)
{
  // Register handlers and configure websocketpp
  m_Server->init_asio();
  m_Server->set_open_handler(std::bind(&AutoNetServer::OnOpen, this, ::_1));
  m_Server->set_close_handler(std::bind(&AutoNetServer::OnClose, this, ::_1));
  m_Server->set_message_handler(std::bind(&AutoNetServer::OnMessage, this, ::_1, ::_2));
  
  // Generate lists of types from type registry
  for (auto type = g_pFirstEntry; type; type = type->pFlink) {
    if (type->IsEventReceiver())
      m_EventTypes.insert(type->NewTypeIdentifier());
    
    if (type->CanInject()) {
      m_AllTypes[type->ti.name()] = [type]{
        type->Inject();
      };
    }
  }
}

AutoNetServer::~AutoNetServer(){}

// CoreThread overrides
void AutoNetServer::Run(void){
  std::cout << "Starting Autonet server..." << std::endl;

  m_Server->listen(m_Port);
  m_Server->start_accept();
  auto websocket = std::async(std::launch::async,[this]{
    m_Server->run();
  });
  
  
  PollThreadUtilization(std::chrono::milliseconds(1000));
  CoreThread::Run();
}

void AutoNetServer::OnStop(void) {
  m_Server->stop();
}

// Server Handler functions
void AutoNetServer::OnOpen(websocketpp::connection_hdl hdl) {
  *this += [this, hdl] {
    SendMessage(hdl, "opened");
  };
}
void AutoNetServer::OnClose(websocketpp::connection_hdl hdl) {
  *this += [this, hdl] {
    SendMessage(hdl, "closed");
    this->m_Subscribers.erase(hdl);
  };
}

void AutoNetServer::OnMessage(websocketpp::connection_hdl hdl, message_ptr p_message) {
  // Parse string from client
  std::string err;
  Json msg = Json::parse(p_message->get_payload(), err);
  
  if (!err.empty()) {
    std::cout << "Parse error: " << err << std::endl;
    SendMessage(hdl, "invalidMessage", "Couldn't parse message");
    return;
  }
  
  std::string msgType = msg["type"].string_value();
  Json::array msgArgs = msg["args"].array_items();

  *this += [this, hdl, msgType, msgArgs] {
    if(msgType == "subscribe") HandleSubscribe(hdl);
    else if(msgType == "unsubscribe") HandleUnsubscribe(hdl);
    else if(msgType == "terminateContext") HandleTerminateContext(msgArgs[0].int_value());
    else if(msgType == "injectContextMember") HandleInjectContextMember(msgArgs[0].int_value(), msgArgs[1].string_value());
    else if(msgType == "resumeFromBreakpoint") HandleResumeFromBreakpoint(msgArgs[0].string_value());
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
    Json::object context {
      {"name", ctxt->GetSigilType().name()}
    };

    if (ctxt->GetParentContext()){
      context["parent"] = ResolveContextID(ctxt->GetParentContext().get());
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
    Json::object objData;
    Json::object types;
    std::shared_ptr<Object> objectPtr(*object);

    // Add object data
    objData["name"] = typeid(*objectPtr).name();
    {
      Json::array slots;
      for (auto slot = object->GetSlotInformation().pHead; slot; slot = slot->pFlink) {
        slots.push_back(Json::object{
          {"name", slot->type.name()},
          {"autoRequired", slot->autoRequired},
          {"offset", int(slot->slotOffset)}
        });
      }
      objData["slots"] = slots;
    }
    
    // Add type information
    auto member = autowiring::fast_pointer_cast<ContextMember>(objectPtr);
    if (member) {
      types["contextMember"] = true;
    }

    auto runnable = autowiring::fast_pointer_cast<CoreRunnable>(objectPtr);
    if (runnable) {
      types["coreRunnable"] = true;
    }
    
    auto thread = autowiring::fast_pointer_cast<BasicThread>(objectPtr);
    if(thread) {
      // Create slot in map
      m_Threads[thread->GetSelf<BasicThread>()];
      
      types["thread"] = Json::object{
        {"kernal", 0.0},
        {"user", 0.0}
      };
    }

    auto eventRcvr = autowiring::fast_pointer_cast<EventReceiver>(objectPtr);
    if (eventRcvr) {
      Json::array listenerTypes;
      for (auto event : m_EventTypes) {
        if (event->Is(objectPtr.get()))
          listenerTypes.push_back(event->Type().name());
      }

      types["eventReceiver"] = listenerTypes;
    }

    auto filter = autowiring::fast_pointer_cast<ExceptionFilter>(objectPtr);
    if (filter) {
      types["exceptionFilter"] = true;
    }

    auto bolt = autowiring::fast_pointer_cast<BoltBase>(objectPtr);
    if (bolt) {
      Json::array sigils;
      for(auto cur = bolt->GetContextSigils(); *cur; cur++){
        sigils.push_back((*cur)->name());
      }
      types["bolt"] = sigils;
    }
  
    BroadcastMessage("newObject", contextID, types, objData);
  };
}

void AutoNetServer::EventFired(CoreContext& context, const std::type_info& info){
  int contextID = ResolveContextID(&context);
  std::string name = info.name();

  *this += [this, contextID, name] {
    BroadcastMessage("eventFired", contextID, Json::object{{"name", name}});
  };
}

void AutoNetServer::HandleSubscribe(websocketpp::connection_hdl hdl) {
  m_Subscribers.insert(hdl);
  
  Json::array types;
  for (auto type : m_AllTypes) {
    types.push_back(type.first);
  }
  
  SendMessage(hdl, "subscribed", types);
  AutoGlobalContext()->BuildCurrentState();
  
  // Send breakpoint message
  for (auto& breakpoint : m_breakpoints) {
    SendMessage(hdl, "breakpoint", breakpoint);
  }
}

void AutoNetServer::HandleUnsubscribe(websocketpp::connection_hdl hdl) {
  this->m_Subscribers.erase(hdl);
  SendMessage(hdl, "unsubscribed");
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
