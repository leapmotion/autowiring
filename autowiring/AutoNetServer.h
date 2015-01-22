// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreThread.h"
#include "ConfigRegistry.h"
#include <sstream>
#include STL_UNORDERED_MAP
#include FUNCTIONAL_HEADER

class AutoNetServer;

extern AutoNetServer* NewAutoNetServerImpl(void);

class AutoNetServer:
  public CoreThread
{
protected:
  AutoNetServer();

public:
  virtual ~AutoNetServer();

  static AutoNetServer* New(void) {
    return NewAutoNetServerImpl();
  }

  /// <summary>
  /// Waits until resume message is sent from the visualizer
  /// </summary>
  /// <param name="name">The identifier for this breakpoint</param>
  virtual void Breakpoint(std::string name) = 0;

  /// Allows a breakpoint previously set with Breakpoint to be resumed
  virtual void HandleResumeFromBreakpoint(std::string name) = 0;
  
  /// Add a custom event handler. Arguments must be primative types are strings
  template<typename... Args>
  void AddEventHandler(const std::string& event, std::function<void(Args...)> handler) {
    AddEventHandler(event, [this, handler] (const std::vector<std::string>& args) {
      assert(sizeof...(Args) == args.size());
    });
  }
  
  /// Send a custom event to all clients.
  template<typename... Args>
  void SendEvent(const std::string& event, Args... args) {
    SendEvent(event, std::vector<std::string>{parse(args)...});
  }
  
protected:
  // Send event with arguments parsed as vector of string
  virtual void SendEvent(const std::string& event, const std::vector<std::string>& args) = 0;
  
  // Add a handler that will be called when an event is received from the client
  void AddEventHandler(const std::string& event, std::function<void(const std::vector<std::string>&)> handler);
  
  // Map of callbacks keyed by event type
  std::unordered_map<std::string, std::vector<std::function<void(std::vector<std::string>)>>> m_handlers;
  
private:
  // parse type to string
  template<class T>
  std::string parse(const T& t){
    std::istringstream ss;
    t >> ss;
    return ss.str();
  }
  
  // parse string to primative type
  template<class T>
  T parse(const std::string& str){
    std::istringstream ss(str);
    T val;
    ss >> std::boolalpha >> val;
    
    if (ss.fail()) {
      autowiring::ThrowFailedTypeParseException(str, typeid(T));
    }
    
    return val;
  }
};
