// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/CoreThread.h>
#include <autowiring/Decompose.h>
#include <autowiring/index_tuple.h>
#include <sstream>
#include STL_UNORDERED_MAP
#include FUNCTIONAL_HEADER

class AutoNetTransportHandler {
public:
  typedef std::weak_ptr<void> connection_hdl;

  virtual void OnOpen(connection_hdl handle) = 0;
  virtual void OnClose(connection_hdl handle) = 0;
  virtual void OnMessage(connection_hdl handle, const std::string& message) = 0;
};

/// <summary>
/// Represents a transport implementation that carries AutoNet data to a visualizer client
/// </summary>
class AutoNetTransport {
public:
  virtual ~AutoNetTransport(void){}

  /// <summary>
  /// Causes the transport to begin servicing user requests
  /// </summary>
  virtual void Start(void) = 0;

  /// <summary>
  /// Causes the transport to stop servicing user requests, also shuts down any active connections
  /// </summary>
  virtual void Stop(void) = 0;

  /// <summary>
  /// Transmits the specified string message to the remote host
  /// </summary>
  virtual void Send(AutoNetTransportHandler::connection_hdl hdl, const std::string& msg) = 0;

  /// <summary>
  /// Assigns the handler for operations occuring on this transport
  /// </summary>
  /// <remarks>
  /// This routine accepts a nullptr argument, the effect will be to clear the current handler
  /// </remarks>
  virtual void SetTransportHandler(std::shared_ptr<AutoNetTransportHandler> handler) = 0;
};

class AutoNetServer;
extern AutoNetServer* NewAutoNetServerImpl(std::unique_ptr<AutoNetTransport>);
extern AutoNetServer* NewAutoNetServerImpl(void);

class AutoNetServer:
  public CoreThread
{
protected:
  AutoNetServer();

public:
  virtual ~AutoNetServer();

  static AutoNetServer* New(std::unique_ptr<AutoNetTransport> transport) {
    return NewAutoNetServerImpl(std::move(transport));
  }

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
