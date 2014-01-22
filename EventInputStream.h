#pragma once
#include <string>
#include <sstream>
#include <memory>
#include <deque>
#include <map>
#include <typeinfo>
#include "Decompose.h"

#include SHARED_PTR_HEADER
#include TYPE_TRAITS_HEADER

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign<decltype(x), x> (#x) 
#endif

template <class T>
class AutoFired;

template <typename T>
const std::string * deser(std::string & str){
  const std::string * ret(&str);
    return ret;
  }

/// <summary>
/// Wrap up memfns as shared_ptrs to ExpressionBase-derived classes. Call func = call wrapped event firing.
/// </summary>
struct ExpressionBase{
  virtual void passmethething(std::deque<std::string> &) =0;
};

template <class MemFn>
struct Expression;

template <class R, class W, typename... ToBindArgs>
struct Expression<R(W::*)(ToBindArgs...) >:
  public ExpressionBase
{
  typedef R(W::*memType)(ToBindArgs...);
  memType m_memfunc;
  Expression(memType m){ m_memfunc = m; }

  void passmethething(std::deque<std::string> & d){ passed(d); }
    
  template <typename ... InputArgs >
  void infunc(InputArgs ... inargs){
    AutoFired<W> sender;
    sender(m_memfunc)(deser<ToBindArgs>(inargs)...);
  }

  typedef std::integral_constant<std::size_t, sizeof ... ( ToBindArgs)> my_arity;
  typedef std::integral_constant<std::size_t, 0 > m_zero;
  typedef std::integral_constant<std::size_t, 1 > m_one;
  typedef std::integral_constant<std::size_t, 2 > m_two;
  typedef std::integral_constant<std::size_t, 3 > m_three;
  typedef std::integral_constant<std::size_t, 4 > m_four;
  typedef std::integral_constant<std::size_t, 5 > m_five;

  
  template <class T = my_arity>
  typename std::enable_if< std::is_same<T,  m_zero>::value, void>::type
    passed(std::deque<std::string>  & d)
  {
    infunc();
  }
  
  template <class T = my_arity>
  typename std::enable_if< std::is_same<T, m_one>::value, void>::type
    passed(std::deque<std::string>  & d)
  {
      infunc(d[1]);
  }

  template <class T = my_arity>
  typename std::enable_if< std::is_same<T, m_two>::value, void>::type
    passed(std::deque<std::string>  & d)
  {
      infunc(d[1], d[2]);
  }

  template <class T = my_arity>
  typename std::enable_if< std::is_same<T, m_three>::value, void>::type
    passed(std::deque<std::string>  & d)
  {
      infunc(d[1], d[2], d[3]);
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
  template<class MemFn>
  bool IsEnabled(MemFn eventIden, bool amIRegistered = false) {
    static int registration = 0; //first time func is checked
    if (amIRegistered && registration < 1){
      registration++;
    }
    return (!!registration);
  }
 
  /// <summary>
  /// Enables a new event for deserialization via its identity
  /// </summary>
  template<class MemFn, MemFn eventIden>
  void SpecialAssign(std::string str) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);
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
    auto chptr = static_cast <const char *> (pData);
    std::string MyString (chptr);

    std::size_t location = MyString.find("\xDE");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> d;
    std::istringstream buf(topevent);

    std::string s;
    while (std::getline(buf, s, '\xD8'))
        d.push_back(s);

    std::string query = d[0];

    auto find1 = m_EventMap.find(query);
    if (find1 != m_EventMap.end()) 
    {
      auto evt = find1 -> second;       
      evt->passmethething(d);        
    }
    return location + 1;
  }
};