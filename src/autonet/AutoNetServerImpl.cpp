// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServerImpl.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/at_exit.h>
#include <autowiring/demangle.h>
#include "AutoNetTransportHttp.hpp"
#include <iostream>
#include FUTURE_HEADER

using json11::Json;

#ifdef _MSC_VER
  // Because Windows is dumb
  #undef SendMessage
#endif

AutoNetServerImpl::AutoNetServerImpl(void):
  AutoNetServerImpl(std::unique_ptr<AutoNetTransportHttp>(new AutoNetTransportHttp))
{
  auto* pCtxt = AutoCurrentContext().get();
  pCtxt->newContext += [this] (CoreContext* pChild) { NewContext(*pChild); };
  pCtxt->expiredContext += [this, pCtxt] { ExpiredContext(*pCtxt); };
  pCtxt->newObject += [this, pCtxt] (const CoreObjectDescriptor& desc) { NewObject(*pCtxt, desc); };
}

AutoNetServerImpl::AutoNetServerImpl(std::unique_ptr<AutoNetTransport>&& transport) :
  m_transport(std::move(transport))
{
  // Register internal event handlers
  AddEventHandler("terminateContext", [this] (int contextID) {
    ResolveContextID(contextID)->SignalShutdown();
  });

  AddEventHandler("injectContextMember", [this] (int contextID, const std::string& typeName){
    std::shared_ptr<CoreContext> ctxt = ResolveContextID(contextID)->shared_from_this();

    if(m_AllTypes.find(typeName) != m_AllTypes.end()) {
      CurrentContextPusher pshr(ctxt);
      m_AllTypes[typeName]();
    }
    else {
      // Type doesn't exist
      assert(false);
    }
  });

  AddEventHandler("resumeFromBreakpoint", [this] (const std::string& name){
    std::lock_guard<std::mutex>{ m_breakpoint_mutex },
    m_breakpoints.erase(name);

    m_breakpoint_cv.notify_all();
  });

  // Generate list of all types from type registry
  for(auto type = g_pFirstTypeEntry; type; type = type->pFlink)
    if(type->CanInject())
      m_AllTypes[autowiring::demangle(type->ti)] = [type]{ type->Inject(); };
}

AutoNetServerImpl::~AutoNetServerImpl(){}

AutoNetServer* NewAutoNetServerImpl(std::unique_ptr<AutoNetTransport> transport) {
  return new AutoNetServerImpl(std::move(transport));
}

AutoNetServer* NewAutoNetServerImpl(void) {
  return new AutoNetServerImpl();
}

// CoreThread overrides
void AutoNetServerImpl::Run(void){
  std::cout << "Starting Autonet server..." << std::endl;

  // Register ourselves as a handler
  m_transport->SetTransportHandler(std::static_pointer_cast<AutoNetServerImpl>(shared_from_this()));

  // blocks until the server finishes
  auto websocket = std::async(std::launch::async, [this]{
    m_transport->Start();
  });

  PollThreadUtilization(std::chrono::milliseconds(1000));

  try {
    CoreThread::Run();
  }
  catch (...) {
    Stop();
    throw;
  }
}

void AutoNetServerImpl::OnStop(void) {
  m_transport->Stop();
}

void AutoNetServerImpl::OnMessage(AutoNetTransportHandler::connection_hdl hdl, const std::string& payload) {
  // Parse string from client
  std::string err;
  Json msg = Json::parse(payload, err);

  if(!err.empty()) {
    std::cout << "Parse error: " << err << std::endl;
    SendMessage(hdl, "invalidMessage", "Couldn't parse message");
    return;
  }

  std::string msgType = msg["type"].string_value();
  Json::array msgArgs = msg["args"].array_items();

  // Handle client specific internal events
  if (msgType == "subscribe") {
    *this += [this, hdl] {
      HandleSubscribe(hdl);
    };
  }
  else if(msgType == "unsubscribe") {
    *this += [this, hdl] {
      HandleUnsubscribe(hdl);
    };
  }
  // Handle other internal events and custom events
  else {
    *this += [this, msgType, msgArgs] {
      // parse args into vector of strings
      std::vector<std::string> args;
      for (const auto& a : msgArgs) {
        args.push_back(!a.string_value().empty() ? a.string_value() : a.dump());
      }

      // call all the handlers
      for (const auto& handler : this->m_handlers[msgType]) {
        handler(args);
      }
    };
  }
}

void AutoNetServerImpl::Breakpoint(std::string name){
  std::unique_lock<std::mutex> lk(m_breakpoint_mutex);

  m_breakpoints.insert(name);

  *this += [this, name]{
    BroadcastMessage("breakpoint", name);
  };

  m_breakpoint_cv.wait(lk, [this, name]{
    return !m_breakpoints.count(name);
  });
}

// Update Functions
void AutoNetServerImpl::NewContext(CoreContext& newCtxt){
  auto ctxt = newCtxt.shared_from_this();

  *this += [this, ctxt] {
    Json::object context{
        {"name", autowiring::demangle(ctxt->GetSigilType())}
    };

    if(ctxt->GetParentContext()){
      context["parent"] = ResolveContextID(ctxt->GetParentContext().get());
    }

    BroadcastMessage("newContext", ResolveContextID(ctxt.get()), context);
  };
}

void AutoNetServerImpl::ExpiredContext(CoreContext& oldCtxt){
  int id = ResolveContextID(&oldCtxt);
  *this += [this, id] {
    BroadcastMessage("expiredContext", id);
  };
}

void AutoNetServerImpl::NewObject(CoreContext& ctxt, const CoreObjectDescriptor& object){
  int contextID = ResolveContextID(&ctxt);

  *this += [this, object, contextID]{
    Json::object objData;
    Json::object types;

    // Add object data
    objData["name"] = autowiring::demangle(object.type);
    objData["id"] = autowiring::demangle(object.value.type());

    // Add slots for this object
    {
      Json::array slots;
      for(auto slot = object.stump->pHead; slot; slot = slot->pFlink) {
        slots.push_back(Json::object{
            {"id", autowiring::demangle(slot->type)},
            {"autoRequired", slot->autoRequired},
            {"offset", int(slot->slotOffset)}
        });
      }
      objData["slots"] = slots;
    }

    // Add type information
    auto member = object.pContextMember;
    if(member) {
      types["contextMember"] = true;
    }

    auto runnable = object.pCoreRunnable;
    if(runnable) {
      types["coreRunnable"] = true;
    }

    auto thread = object.pBasicThread;
    if(thread) {
      // Create slot in map
      m_Threads[thread->GetSelf<BasicThread>()];

      types["thread"] = Json::object{
          {"kernal", 0.0},
          {"user", 0.0}
      };
    }

    // Check if type implements an AutoFilter
    if (!object.subscriber.empty()) {
      Json::object args;
      for (auto pArg = object.subscriber.GetAutoFilterArguments(); *pArg; ++pArg) {
        args[autowiring::demangle(pArg->id)] = Json::object{
          {"id", autowiring::demangle(pArg->id)},
          {"isInput", pArg->is_input},
          {"isOutput", pArg->is_output}
        };
      }
      types["autoFilter"] = args;
    }

    auto filter = object.pFilter;
    if(filter) {
      types["exceptionFilter"] = true;
    }

    auto bolt = object.pBoltBase;
    if(bolt) {
      Json::array sigils;
      for(auto cur = bolt->GetContextSigils(); *cur; cur++){
        sigils.push_back(autowiring::demangle(*cur));
      }
      types["bolt"] = sigils;
    }

    BroadcastMessage("newObject", contextID, types, objData);
  };
}

void AutoNetServerImpl::SendEvent(const std::string& rawEvent, const std::vector<std::string>& args) {
  // Prepend '$' to custum event to avoid namespace collitions with internal events
  std::string event("$");
  event.append(rawEvent);

  Json::array jsonArgs;
  for (const auto& a : args) {
    jsonArgs.push_back(a);
  }

  *this += [this, event, jsonArgs] {
    for(auto hdl : m_Subscribers) {
      Json msg = Json::object{
        {"type", event},
        {"args", jsonArgs}
      };

      m_transport->Send(hdl, msg.dump());
    }
  };
}

void AutoNetServerImpl::HandleSubscribe(websocketpp::connection_hdl hdl) {
  m_Subscribers.insert(hdl);

  Json::array types;
  for(const auto& type : m_AllTypes) {
    types.push_back(type.first);
  }

  SendMessage(hdl, "subscribed", types);
  GetContext()->BuildCurrentState();

  // Send breakpoint message
  for(const auto& breakpoint : m_breakpoints) {
    SendMessage(hdl, "breakpoint", breakpoint);
  }
}

void AutoNetServerImpl::HandleUnsubscribe(websocketpp::connection_hdl hdl) {
  this->m_Subscribers.erase(hdl);
  SendMessage(hdl, "unsubscribed");
}

//helper functions
int AutoNetServerImpl::ResolveContextID(CoreContext* ctxt) {
  static int counter = 0;

  if(m_ContextIDs.find(ctxt) == m_ContextIDs.end()){
    m_ContextIDs[ctxt] = counter;
    m_ContextPtrs[counter] = ctxt;
    return counter++;
  }
  else {
    return m_ContextIDs[ctxt];
  }
}

CoreContext* AutoNetServerImpl::ResolveContextID(int id) {
  return m_ContextPtrs.at(id);
}

void AutoNetServerImpl::PollThreadUtilization(std::chrono::milliseconds period){

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

      auto& threadRef = *thread;
      std::string name = autowiring::demangle(typeid(threadRef));

      std::chrono::duration<double> periodDbl = period;
      double kmPercent = 100.0 * (deltaRuntimeKM.count() / periodDbl.count());
      double umPercent = 100.0 * (deltaRuntimeUM.count() / periodDbl.count());

      // Make sure user + kernel percent < 100.0
      umPercent = std::min(umPercent, 99.9 - kmPercent);

      if(kmPercent >= 0.0 && umPercent >= 0.0) {
        BroadcastMessage("threadUtilization", contextID, name, kmPercent, umPercent);
      }

      // Next!
      q++;
    }

    // Poll again after "period" milliseconds
    PollThreadUtilization(period);
  };
}
