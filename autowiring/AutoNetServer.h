// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreThread.h"
#include "ConfigRegistry.h"
#include "index_tuple.h"
#include <autowiring/Decompose.h>
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
  
  /// Add a custom event handler. Arguments must be primative types are strings
  template<typename Fx>
  void AddEventHandler(const std::string& event, Fx&& handler) {
    AddEventHandler<Fx>(
      event,
      std::forward<Fx&&>(handler),
      &Fx::operator(),
      typename make_index_tuple<Decompose<decltype(&Fx::operator())>::N>::type()
    );
  }
  
  /// Send a custom event to all clients.
  template<typename... Args>
  void SendEvent(const std::string& event, Args... args) {
    SendEvent(event, std::vector<std::string>{parseToString(args)...});
  }
  
protected:
  // Send event with arguments parsed as vector of string
  virtual void SendEvent(const std::string& event, const std::vector<std::string>& args) = 0;
  
  // Map of callbacks keyed by event type
  std::unordered_map<std::string, std::vector<std::function<void(std::vector<std::string>)>>> m_handlers;
  
private:
  // Add a handler that will be called when an event is received from the client
  void AddEventHandlerInternal(const std::string& event, std::function<void(const std::vector<std::string>&)> handler);
  
  // Extract arguments from list of strings, parse and pass to handler
  template<typename Fx, typename... Args, int... N>
  void AddEventHandler(const std::string& event, Fx&& handler, void (Fx::*pfn)(Args...) const, index_tuple<N...>) {
    AddEventHandlerInternal(
      event,
      [this, handler, pfn] (const std::vector<std::string>& args) {
        if (sizeof...(Args) != args.size())
          // TODO:  Return some kind of singal to the caller indicating that there is a problem
          return;
        (handler.*pfn)(parseFromString<typename std::decay<Args>::type>(args[N])...);
      }
    );
  }
  
  // parse type to string
  template<class T>
  std::string parseToString(const T& t){
    std::ostringstream ss;
    ss << t;
    return ss.str();
  }
  
  // parse string to primative type
  template<class T>
  inline T parseFromString(const std::string& str){
    std::istringstream ss(str);
    typename std::decay<T>::type val;
    ss >> std::boolalpha >> val;
    
    if (ss.fail()) {
      autowiring::ThrowFailedTypeParseException(str, typeid(T));
    }
    
    return val;
  }
};

// template specialization for a string to just return itself unparsed
template<>
inline std::string AutoNetServer::parseFromString<std::string>(const std::string& str) {
  return str;
}
