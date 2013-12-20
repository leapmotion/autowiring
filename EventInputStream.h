#pragma once
#include <string>
#include <deque>
#include <map>
#include <typeinfo>
#include "Autowired.h"

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign(#x, x) 
#endif
/// <summary>
/// Allows the deserialization of events from an output stream, in order to replay them in-process
/// </summary>
template<class T>
class EventInputStream
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot instantiate an event input stream on a non-event type");
  std::map<std::string, std::type_info *> my_type_info_map;

  /// <summary>
  /// helper function for memfn querying
  // </summary>


  /// <summary>
  /// Converts strings to their proper memberfunctions for deserialization and firing.
  /// </summary>
  template <std::type_info * ti, class MemFn>
  MemFn
  AddAndQueryMemFn(std::type_info * ti, MemFn memfn = nullptr)
  {
  std::map<std::string, MemFn> local; 
  static std::map<std::string, MemFn> my_map = local;
  if (memfn == nullptr)
  {
    auto find = my_map.find(str);
    if (find != my_map.end()) return find->second;
  }
  my_map[str] = memfn;
  return nullptr;
  }

private:
  typedef void (T::*fnPtr)(std::string); 
  std::map<std::string, fnPtr> m_oneArgsMap;
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
  template<class MemFn>
  void SpecialAssign(std::string str, MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");

    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);
      //my_type_info_map[str] = &typeid(eventIden);
      //AddAndQueryMemFn<&typeid(eventIden)> (&typeid(eventIden), eventIden);
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
    std::string MyString (chptr, dataSize);
    //std::cout<<"Fire Single was called" << std ::endl;
    //std::cout<< MyString;
    std::size_t location = MyString.find("Þ");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> v;
    std::istringstream buf(topevent);
    for(std::string token; getline(buf, token, 'Ø'); )
        v.push_back(token);

    //stuck here. almost done.
    std::string query = v[0];
    //auto EventPredicate = AddAndQueryMemFn(*my_type_info_map.find(query));
   // EventPredicate(v[1]);
    //for(auto it = v.begin(); it != v.end(); ++it)
        //std::cout<<*it << std::endl;
    /*
    for (unsigned int i = 0; i < dataSize; ++i)
        std::cout << ((char *)pData)[i];
        */
    return dataSize - location;
  }
};

