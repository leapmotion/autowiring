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

//The point here is to use template specialization to pick default-deserialization for certain types at registration 

template <typename T>
//struct DeserializeHelper{
   static T Deserialize(std::string & str){
    T arg1;
    std::stringstream buf;
    buf << str;
    buf >> arg1;
    return arg1;
  }
//};

//The point here is to use template specialization to pick default-deserialization for certain types at registration 
template <typename T>
//struct DeserializeHelper<const T * >{
  const T * Deserialize(std::string & str){
    const T * ret = &str;
    return ret;
  }
//};

template <int n>
struct CompileTimeInt{};

struct NoDo{
   void * Deserialize(std::string & str){return nullptr;}
};


/// <summary>
/// Wrap up memfns as shared_ptrs to ExpressionBase-derived classes. Call func = call wrapped event firing.
/// </summary>
struct ExpressionBase{

  template <typename ... InputArgs >
  void func(InputArgs ... inargs)
  {
    std::cout << "Boo the wrong one was called!" << std::endl;
  }
};

template <class MemFn>
struct Expression;

template <class R, class W, typename... ToBindArgs>
struct Expression<R(W::*)(ToBindArgs...) >:
  public ExpressionBase
{
    
  template <typename ... InputArgs >
  void func(InputArgs ... inargs){
    std::cout << "YAY right one was called!" << std::endl;

    AutoFired<T> sender;
    sender(memfn)(Deserialize<ToBindArgs>(inargs)...);
  }

};


/*Critical Insight:

template<class MemFn>
struct MyDecompose;

template<class R, class W, typename... Targs>
struct MyDecompose<R(W::*)(Targs...)> {
static const std::tuple<Targs...> my_args;
static const std::size_t Arity = std::tuple_size<decltype(my_args)>::value;

};

template <typename ... ToBindArgs, typename ... InputArgs >
void func(InputArgs ... inargs){

FuncThatAcceptsDeserializedArgs(Deserialize<ToBindArgs>(inargs)...);
}
*/

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
      std::shared_ptr<ExpressionBase> ptr = std::make_shared<Expression<MemFn> >();
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

    //null-arg pad the dequeso it has size = 5
    //while (d.size() != 5) d.push_back(std::string(""));


    std::string query = d[0];

    auto find1 = m_EventMap.find(query);
    if (find1 != m_EventMap.end()) 
    {
      auto evt = find1 -> second;
      switch (d.size()){
      //Dynamicaly pack up args to send to variadics
      case 1: //noargs 
        evt->func();
        break;
      case 2: //noargs 
        evt->func(d[1]);
        break;
      case 3: //noargs 
        evt->func(d[1], d[2]);
        break;
      case 4: //noargs 
        evt->func(d[1], d[2], d[3]);
        break;
      case 5: //noargs 
        evt->func(d[1], d[2], d[3], d[4]);
        break;
      case 6: //noargs 
        evt->func(d[1], d[2], d[3], d[4], d[5]);
        break;
      case 7: //noargs 
        evt->func(d[1], d[2], d[3], d[4], d[5], d[6]);
        break;
      }
    }
    return location +1 ;
  }
};