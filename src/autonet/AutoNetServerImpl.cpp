// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServerImpl.hpp"
#include "at_exit.h"
#include "autowiring.h"
#include "demangle.h"
#include "ObjectTraits.h"
#include "EventRegistry.h"
#include "TypeRegistry.h"
#include <iostream>
#include FUTURE_HEADER

using std::placeholders::_1;
using std::placeholders::_2;
using json11::Json;

template<typename ASIO>
AutoNetServerImpl<ASIO>::AutoNetServerImpl(void) :
  m_Port(8000)
{
  // Configure websocketpp
  m_Server.init_asio();
  m_Server.set_access_channels(websocketpp::log::alevel::none);
  m_Server.set_error_channels(websocketpp::log::elevel::none);

  // Register handlers
  m_Server.set_open_handler(std::bind(&AutoNetServerImpl::OnOpen, this, ::_1));
  m_Server.set_close_handler(std::bind(&AutoNetServerImpl::OnClose, this, ::_1));
  m_Server.set_message_handler(std::bind(&AutoNetServerImpl::OnMessage, this, ::_1, ::_2));
  
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
    std::lock_guard<std::mutex> lk(m_breakpoint_mutex);
    
    m_breakpoints.erase(name);
    m_breakpoint_cv.notify_all();
  });

  // Generate list of all types from type registry
  for(auto type = g_pFirstTypeEntry; type; type = type->pFlink)
    if(type->CanInject())
      m_AllTypes[autowiring::demangle(type->ti)] = [type]{ type->Inject(); };

  // Generate list of all events from event registry
  for(auto event = g_pFirstEventEntry; event; event = event->pFlink)
    m_EventTypes.insert(event->NewTypeIdentifier());
}

template<typename ASIO>
AutoNetServerImpl<ASIO>::~AutoNetServerImpl(){}

AutoNetServer* NewAutoNetServerImpl(AutoNetSecurity level) {
  if (level == AutoNetSecurity::TLS) {
    return new AutoNetServerImpl<websocketpp::config::asio_tls>();
  } else {
    return new AutoNetServerImpl<websocketpp::config::asio>();
  }
}

// CoreThread overrides
template<typename ASIO>
void AutoNetServerImpl<ASIO>::Run(void){
  std::cout << "Starting Autonet server..." << std::endl;
  
  m_Server.listen(m_Port);
  m_Server.start_accept();
  
  // blocks until the server finishes
  auto websocket = std::async(std::launch::async, [this]{
    m_Server.run();
  });

  PollThreadUtilization(std::chrono::milliseconds(1000));
  CoreThread::Run();
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::OnStop(void) {
  if (m_Server.is_listening())
    m_Server.stop_listening();
  
  for (auto& conn : m_Subscribers) {
    m_Server.close(conn, websocketpp::close::status::normal, "closed");
  }
}

// Server Handler functions
template<typename ASIO>
void AutoNetServerImpl<ASIO>::OnOpen(websocketpp::connection_hdl hdl) {
  *this += [this, hdl] {
    SendMessage(hdl, "opened");
  };
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::OnClose(websocketpp::connection_hdl hdl) {
  *this += [this, hdl] {
    this->m_Subscribers.erase(hdl);
  };
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::OnMessage(websocketpp::connection_hdl hdl, message_ptr p_message) {
  // Parse string from client
  std::string err;
  Json msg = Json::parse(p_message->get_payload(), err);

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

// TLS Certificate
template<>
void AutoNetServerImpl<websocketpp::config::asio>::SetCertificate(std::shared_ptr<autoboost::asio::ssl::context>) {}

// TLS Certificate
template<>
void AutoNetServerImpl<websocketpp::config::asio_tls>::SetCertificate(std::shared_ptr<autoboost::asio::ssl::context> certificate) {
  m_Server.set_tls_init_handler([certificate] (websocketpp::connection_hdl) {
    return certificate;
  });
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::Breakpoint(std::string name){
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
template<typename ASIO>
void AutoNetServerImpl<ASIO>::NewContext(CoreContext& newCtxt){
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

template<typename ASIO>
void AutoNetServerImpl<ASIO>::ExpiredContext(CoreContext& oldCtxt){
  int id = ResolveContextID(&oldCtxt);
  *this += [this, id] {
    BroadcastMessage("expiredContext", id);
  };
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::NewObject(CoreContext& ctxt, const ObjectTraits& object){
  int contextID = ResolveContextID(&ctxt);

  *this += [this, object, contextID]{
    Json::object objData;
    Json::object types;

    // Add object data
    objData["name"] = autowiring::demangle(object.type);
    objData["id"] = autowiring::demangle(object.value.slot()->type());

    // Add slots for this object
    {
      Json::array slots;
      for(auto slot = object.stump.pHead; slot; slot = slot->pFlink) {
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
      for (auto pArg = object.subscriber.GetAutoFilterInput(); *pArg; ++pArg) {
        args[autowiring::demangle(pArg->ti)] = Json::object{
          {"id", autowiring::demangle(pArg->ti)},
          {"isInput", pArg->is_input},
          {"isOutput", pArg->is_output}
        };
      }
      types["autoFilter"] = args;
    }

    // Check if type receives any events
    {
      Json::array listenerTypes;
      for(const auto& event : m_EventTypes) {
        if(event->IsSameAs(object.pCoreObject.get()))
          listenerTypes.push_back(autowiring::demangle(event->Type()));
      }

      if(!listenerTypes.empty())
        types["eventReceiver"] = listenerTypes;
    }

    auto filter = object.pFilter;
    if(filter) {
      types["exceptionFilter"] = true;
    }

    auto bolt = object.pBoltBase;
    if(bolt) {
      Json::array sigils;
      for(auto cur = bolt->GetContextSigils(); *cur; cur++){
        sigils.push_back(autowiring::demangle(**cur));
      }
      types["bolt"] = sigils;
    }

    BroadcastMessage("newObject", contextID, types, objData);
  };
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::SendEvent(const std::string& rawEvent, const std::vector<std::string>& args) {
  // Prepend '$' to custum event to avoid namespace collitions with internal events
  std::string event("$");
  event.append(rawEvent);
  
  Json::array jsonArgs;
  for (const auto& a : args) {
    jsonArgs.push_back(a);
  }
  
  *this += [this, event, jsonArgs] {
    for(websocketpp::connection_hdl hdl : m_Subscribers) {
      Json msg = Json::object{
        {"type", event},
        {"args", jsonArgs}
      };
      
      m_Server.send(hdl, msg.dump(), websocketpp::frame::opcode::text);
    }
  };
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::HandleSubscribe(websocketpp::connection_hdl hdl) {
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

template<typename ASIO>
void AutoNetServerImpl<ASIO>::HandleUnsubscribe(websocketpp::connection_hdl hdl) {
  this->m_Subscribers.erase(hdl);
  SendMessage(hdl, "unsubscribed");
}

//helper functions
template<typename ASIO>
int AutoNetServerImpl<ASIO>::ResolveContextID(CoreContext* ctxt) {
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

template<typename ASIO>
CoreContext* AutoNetServerImpl<ASIO>::ResolveContextID(int id) {
  return m_ContextPtrs.at(id);
}

template<typename ASIO>
void AutoNetServerImpl<ASIO>::PollThreadUtilization(std::chrono::milliseconds period){

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
      std::string name = autowiring::demangle(typeid(*thread.get()));

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

template class AutoNetServerImpl<websocketpp::config::asio>;
template class AutoNetServerImpl<websocketpp::config::asio_tls>;
