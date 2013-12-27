#pragma once
#include <string>
#include <sstream>
#include <memory>
#include <deque>
#include <map>
#include <typeinfo>
#include "Autowired.h"

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign<decltype(x), x> (#x) 
#endif

template <int n>
struct CompileTimeInt{};

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

struct NoDo{
   CompileTimeInt<666> * Deserialize(std::string & str){return nullptr;}	
};

/// <summary>
/// Pick the right way to fire an event based on the return type of the member function
/// </summary>
template <class T, class MemFn, class ReturnType>
struct DeferOrFire{
};

template <class T, class MemFn>
struct DeferOrFire<T, MemFn, Deferred>
{
  InvokeRelay<MemFn> func(T & t, MemFn memfn)
  {
    return t.Defer(memfn);
  }
};

template <class T, class MemFn>
struct DeferOrFire<T, MemFn, void>
{
  InvokeRelay<MemFn> func(T & t, MemFn memfn)
  {
    return t(memfn);
  }
};

/// <summary>
/// N-Member typedef detector
/// Reflection primitive which deduces 2^4 possible types from Decompose(memfn)
/// Use: auto arg1 = NMemberDetector::select<Test>(nullptr, "12").Deserialize(xx);
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
  virtual void func(std::string = "", std::string = "", std::string = "", std::string = "") = 0;
};

template <class T, class Memfn, Memfn memfn, int n>
struct Expression: public ExpressionBase{
  void func(std::string = "", std::string = "", std::string = "", std::string = ""){
  }
};

template <class T, class Memfn, Memfn memfn>
struct Expression<T, Memfn, memfn, 0>: public ExpressionBase{
  //0 args case. So deserialize the first string, ignore the rest
  void func(std::string s1 = "", std::string s2= "", std::string s3= "", std::string s4 = ""){
    std::cout << "Hi from expression no args " << std::endl;
    AutoFired<T> sender;
    //sender(memfn)();
    DeferOrFire<decltype(sender), Memfn, typename Decompose<Memfn>::retType> FireType;
    FireType.func(sender, memfn)();
  }
};

template <class T, class Memfn, Memfn memfn>
struct Expression<T, Memfn, memfn, 1>: public ExpressionBase{
    void func(std::string s1 = "", std::string s2 = "", std::string s3= "", std::string s4 = ""){      
    AutoFired<T> sender;
    DeferOrFire<decltype(sender), Memfn, typename Decompose<Memfn>::retType> FireType;
    //auto arg1 = DeserializeHelper<typename Decompose<Memfn>::t_arg1>::Deserialize(s1);
    auto arg1 = TypedefDetector::select<typename Decompose<Memfn> >(nullptr, CompileTimeInt<1>()).Deserialize(s1);
    auto arg2 = TypedefDetector::select<typename Decompose<Memfn>>(nullptr, CompileTimeInt<2>()).Deserialize(s2);
    auto arg3 = TypedefDetector::select<typename Decompose<Memfn>>(nullptr, CompileTimeInt<3>()).Deserialize(s3);
    auto arg4 = TypedefDetector::select<typename Decompose<Memfn>>(nullptr, CompileTimeInt<4>()).Deserialize(s4);
    FireType.func(sender, memfn)(arg1);
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
      std::shared_ptr<ExpressionBase> ptr = std::make_shared<Expression<T, MemFn, eventIden, Decompose<MemFn>::N> >();
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

    std::size_t location = MyString.find("Þ");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> d;
    std::istringstream buf(topevent);

    std::string s;
    while (std::getline(buf, s, 'Ø'))
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

