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
struct DeserializeHelper{
   static T Deserialize(std::string & str){
    T arg1;
    std::stringstream buf;
    buf << str;
    buf >> arg1;
    return arg1;
  }
};

//The point here is to use template specialization to pick default-deserialization for certain types at registration 
template <typename T>
struct DeserializeHelper<const T * >{
  const T * Deserialize(std::string & str){
    const T * ret = &str;
    return ret;
  }
};

template <int n>
struct CompileTimeInt{};

struct NoDo{
   void * Deserialize(std::string & str){return nullptr;}
};

/// <summary>
/// N-Member typedef detector
/// Reflection primitive to deduce presence of args from Decompose(memfn)
/// Use: auto arg1 = TypedefDetector::select<Test>(nullptr, CompileTimeInt<n>)
/// </summary>
struct TypedefDetector
{
  template<class U>
  static DeserializeHelper<typename U::t_arg1> select(typename U::t_arg1 *, CompileTimeInt<1>)
  {
  return DeserializeHelper<typename U::t_arg1>();
  }
  
  template<class U>
  static DeserializeHelper<typename U::t_arg2> select(typename U::t_arg2 *, CompileTimeInt<2>)
  {
  return DeserializeHelper<typename U::t_arg2>();
  }  
  
  template<class U>
  static DeserializeHelper<typename U::t_arg3> select(typename U::t_arg3 *, CompileTimeInt<3>)
  {
  return DeserializeHelper<typename U::t_arg3>();
  }  
  
  template<class U>
  static DeserializeHelper<typename U::t_arg4> select(typename U::t_arg4 *, CompileTimeInt<4>)
  {
  return DeserializeHelper<typename U::t_arg4>();
  }

  template<class U>
  static NoDo select(...){
  return NoDo();
  }

};

/// <summary>
/// Wrap up memfns as shared_ptrs to ExpressionBase-derived classes. Call func = call wrapped event firing.
/// </summary>
struct ExpressionBase{
  virtual void func(std::string, std::string, std::string, std::string) = 0;
};

template <class T, class Memfn, Memfn memfn>
struct Expression:
  public ExpressionBase
{
  decltype(TypedefDetector::select<Decompose<Memfn> >(nullptr, CompileTimeInt<1>())) m_arg1;
  decltype(TypedefDetector::select<Decompose<Memfn> >(nullptr, CompileTimeInt<2>())) m_arg2;
  decltype(TypedefDetector::select<Decompose<Memfn> >(nullptr, CompileTimeInt<3>())) m_arg3;
  decltype(TypedefDetector::select<Decompose<Memfn> >(nullptr, CompileTimeInt<4>())) m_arg4;

  template <class A, class B, class C>
  void fire(void *, void *, void *, void *, B & sender, C & FireType) {
    FireType.func(sender, memfn)();
  }

  template <class A, class B, class C>
  void fire(A a1, void *, void *, void *, B & sender, C & FireType) {
    FireType.func(sender, memfn)(a1);
  }

  void func(std::string s1, std::string s2, std::string s3, std::string s4) {}
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
      std::shared_ptr<ExpressionBase> ptr = std::make_shared<Expression<T, MemFn, eventIden> >();
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
    while (d.size() != 5) d.push_back(std::string(""));

    std::string query = d[0];

    auto find1 = m_EventMap.find(query);
    if (find1 != m_EventMap.end()) 
    {
      auto evt = find1 -> second;
       evt -> func(d[1], d[2], d[3], d[4]);
    }
    return location +1 ;
  }
};