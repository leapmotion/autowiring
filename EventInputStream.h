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

template <typename T>
struct DeserializeHelper{
   static T Deserialize(std::string & str){
    T arg1;
    std::stringstream buf;
    buf << s1;
    buf >> arg1;
    return arg1
  }
};

template <typename T>
struct DeserializeHelper<const T * >{
  static const T * Deserialize(std::string & str){
    const T * ret = &str;
    return ret;
  }
};

struct ExpressionBase{
  virtual void func(std::string = "", std::string = "", std::string = "", std::string = "") = 0;
};

template <class T, class Memfn, Memfn memfn, int n>
struct Expression: public ExpressionBase{
  void func(std::string = "", std::string = "", std::string = "", std::string = ""){
    std::cout << "Hi from no init" << std::endl; 
  }
};

template <class T, class Memfn, Memfn memfn>
struct Expression<T, Memfn, memfn, 0>: public ExpressionBase{
  //0 args case. So deserialize the first string, ignore the rest
  void func(std::string s1 = "", std::string s2= "", std::string s3= "", std::string s4 = ""){
    std::cout << "Hi from expression no args " << std::endl;
    AutoFired<T> sender; 
    sender(memfn)();
  }
};

template <class T, class Memfn, Memfn memfn>
struct Expression<T, Memfn, memfn, 1>: public ExpressionBase{
  //typedef Decompose<Memfn>::type Arg1;
  typedef Decompose<Memfn> decompose;
    void
    func(std::string s1 = "", std::string s2 = "", std::string s3= "", std::string s4 = ""){
    AutoFired<T> sender;

    //deserialization
     sender(memfn)(DeserializeHelper<decompose::t_arg1>::Deserialize(s1));
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

  typedef Deferred (T::*DeferredfnPtr)(const std::string *);
  std::map<std::string, DeferredfnPtr> Deferred_map;

private:
  std::shared_ptr<EventReceiverProxy<T>> m_receiver;
  std::map<std::string, std::shared_ptr<ExpressionBase> > test_fired_map;
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
  /// Enables a new FIRED event for deserialization via its identity
  /// </summary>
  template<class MemFn, MemFn eventIden>
  typename std::enable_if< std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, void >::type 
  SpecialAssign(std::string str) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);
      Deferred_map[str] = eventIden;
      //test_fired_map[str] = 
    }
  }

  /// <summary>
  /// Enables a new DEFERRED event for deserialization via its identity
  /// </summary>
  template<class MemFn, MemFn eventIden>
  typename std::enable_if< std::is_same<typename Decompose<MemFn>::retType, void>::value, void >::type  
  SpecialAssign(std::string str) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");

    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);

      std::shared_ptr<ExpressionBase> ptr = std::make_shared<Expression<T, MemFn, eventIden, Decompose<MemFn>::N> >();
      test_fired_map[str] = ptr;
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
    std::string MyString (chptr);//, dataSize);

    std::size_t location = MyString.find("Þ");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> v;
    std::istringstream buf(topevent);

    std::string s;
    while (std::getline(buf, s, 'Ø'))
        v.push_back(s);

    std::string query = v[0];
    const std::string * ARG1 = &v[1];

    AutoFired<T> sender;

    auto find1 = test_fired_map.find(query);
    if (find1 != test_fired_map.end()) 
    {
      auto evt = find1 -> second;
      evt -> func(v[1]);
    }
    

    auto find2 = Deferred_map.find(query);
    if (find2 != Deferred_map.end())
    {
      auto eventpred2 = find2 -> second;
      sender.Defer(eventpred2)(ARG1);
    }

    return location +1 ;
  }
};

