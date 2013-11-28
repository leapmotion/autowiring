#pragma once
#include "Bolt.h"
#include "Autowired.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

/// <summary>
/// Utility template and macro for universal type-based naming system
/// Use: Sigil(somenameofthing) declarethename; 
/// Result: GlobalNameType paramaterized on the name padded with $
/// </summary>
/// <param name="Namespace">The scope within which indexed types will be unique</param>

struct GlobalNameTypeBase{};

template <char * T>
struct teststruct{};

template <char A0 ='$', char A1 ='$', char A2 = '$',  char A3 = '$', char A4 ='$', char A5 ='$',\
          char A6 ='$', char A7 ='$', char A8 ='$', char A9 ='$', char A10 ='$', char A11 ='$',\
          char A12 ='$', char A13 ='$', char A14 ='$', char A15 ='$', char A16 ='$', char A17 ='$',\
          char A18 ='$', char A19 = '$', char A20 = '$'>
struct GlobalNameType : public GlobalNameTypeBase{};

#define STRING(x) #x "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
#define SigilUnsafe(arg) GlobalNameType<arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], \
        arg[6], arg[7], arg[8], arg[9], arg[10], arg[11], arg[12], arg[13], arg[14], arg[15],\
       arg[16], arg[17], arg[18], arg[19], arg[20]>
#define Sigil(arg) SigilUnsafe(STRING(arg))

//static const char tryitout = "Sddssfsdf"[0];
static const GlobalNameType<'a'> argasd;


/// <summary>
/// A utility class which can create monotonic index counters in some namespace
/// </summary>
/// <param name="Namespace">The scope within which indexed types will be unique</param>


template<class Namespace>
struct TypeIndexer {
  template <class T, class X>
  static std::vector<X>& return_head_of_list(void){
    static std::vector<X> first = std::vector<X>(); //Assigned only once per T;
    return first;
  }

  template <class T, class X, X x>
  static int one_time_insert_helper(void){
    auto list = return_head_of_list<T, X>();
    list.push_back (x);
    return 0;
  }
  
  template <class T,  class X, X x>
  static void one_time_insert(void){
    static const int DoThisOnce = one_time_insert_helper<T, X, x>();
  }
};

typedef void(*ctxtfnptr)(std::shared_ptr<CoreContext>);

template <class T>
void make_me_an_auto_func(std::shared_ptr<CoreContext> cptr){
  CurrentContextPusher pshr(cptr);
  AutoRequired<T>;
  return;
}

static  ctxtfnptr a= make_me_an_auto_func<int>;
static  ctxtfnptr b= make_me_an_auto_func<int>;
