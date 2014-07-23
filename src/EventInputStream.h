#pragma once

#include "Decompose.h"
#include "Deserialize.h"
#include <string>
#include <sstream>
#include <deque>
#include <map>

#include MEMORY_HEADER
#include TYPE_TRAITS_HEADER

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign<decltype(x), x> (#x)
#endif

template <class T>
class AutoFired;

/// <summary>
/// Wrap up memfns as shared_ptrs to ExpressionBase-derived classes. Call func = call wrapped event firing.
/// </summary>
struct ExpressionBase{
  virtual void DeserializeAndForward(std::deque<std::string> &) = 0;
};

template <class MemFn>
struct Expression;

template <class R, class W, typename... ToBindArgs>
struct Expression<R(W::*)(ToBindArgs...) >: public ExpressionBase
{
  typedef R(W::*memType)(ToBindArgs...);
  memType m_memfunc;
  Expression(memType m){ m_memfunc = m; }

  /// <summary>
  /// This function deserializes exactly as many arguments from the
  /// argument dequeue as the length of the type pack ToBindArgs by using
  /// parameter pack expansion.
  /// </summary>
  void DeserializeAndForward(std::deque<std::string> & d){
    DeserializeAndForward(d, typename Auto::make_index_tuple<ToBindArgs...>::type());
  }

  template<unsigned... I>
  void DeserializeAndForward(std::deque<std::string> & d, Auto::index_tuple<I...>){
    auto it = d.begin();
    AutoFired<W> sender;
    sender(m_memfunc)(Auto::deser<ToBindArgs>::deserialize(it[I])...);
  }
};

/// <summary>
/// Allows the deserialization of events from an output stream, in order to replay them in-process
/// </summary>
template<class T>
class EventInputStream
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot instantiate an event input stream on a non-event type");

private:
  std::map<std::string, std::shared_ptr<ExpressionBase> > m_EventMap;

public:
  EventInputStream(){}

  /// <summary>
  /// Returns true if memfn is enabled, otherwise false.
  /// </summary>
  bool IsEnabled(std::string str) {
    return !(m_EventMap.find(str) == m_EventMap.end());
  }

  /// <summary>
  /// Enables a new event for deserialization via its identity
  /// </summary>
  template<class MemFn, MemFn eventIden>
  void SpecialAssign(std::string str) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    if (!IsEnabled(str))
    {
      std::shared_ptr<ExpressionBase> ptr = std::make_shared<Expression<MemFn> >(eventIden);
      m_EventMap[str] = ptr;
    }
  }

  /// <summary>
  /// Interprets and fires a SINGLE EVENT from the passed input buffer
  /// </summary>
  /// <returns>
  /// The number of bytes processed from the input buffer
  /// </returns>
  size_t FireSingle(const void* pData, size_t dataSize) const {
    //First wrap all the bytes in a string.
    auto chptr = (const char *)pData;
    std::string MyString(chptr);

    std::size_t location = MyString.find("\xDE");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> d;
    std::istringstream buf(topevent);

    std::string s;
    while(std::getline(buf, s, '\xD8'))
      d.push_back(s);

    std::string query = d[0];
    d.pop_front(); // Now a list of arguments

    auto find1 = m_EventMap.find(query);
    if (find1 != m_EventMap.end())
    {
      auto evt = find1 -> second;
      evt->DeserializeAndForward(d);
    }
    return location + 1;
  }
};