#pragma once
#include "ContextMember.h"
#include "SlotInformation.h"
#include TYPE_TRAITS_HEADER
#include RVALUE_HEADER

template<typename T>
struct has_static_new
{
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static int select(unnamed_constant<U* (*)(), &U::New>*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};

template<typename T, bool isAbstract = std::is_abstract<T>::value>
struct has_simple_constructor
{
  template<class U>
  static int select(decltype(U())*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};

template<typename T>
struct has_simple_constructor<T, true>
{
  static const bool value = false;
};


/// <summary>
/// Simple structure to centralize knowledge about how to create types with various declarations
/// </summary>
struct CreationRules {
  template<typename U>
  static typename std::enable_if<has_static_new<U>::value, U*>::type New(void) {
    return U::New();
  }

  template<typename U, typename... Args>
  static typename std::enable_if<!has_static_new<U>::value, U*>::type New(Args&&... args) {
    static_assert(!std::is_abstract<U>::value, "Cannot create a type which is abstract");
    static_assert(!has_static_new<U>::value, "Can't inject member with arguments if it has a static new");
    static_assert(!sizeof...(Args) || !has_simple_constructor<U>::value, "Can't inject member with arguments if it has a default constructor");

    // Allocate slot first before registration
    auto* pSpace = new unsigned char[sizeof(U)];

    try {
      // Stack location and placement new in one expression
      return
        SlotInformationStackLocation::PushStackLocation<U>(reinterpret_cast<U*>(pSpace)),
        new (pSpace) U(std::forward<Args>(args)...);
    }
    catch(...) {
      // Don't want memory leaks--but we also want to avoid calling the destructor, here, so we cast to void before freeing
      delete[] pSpace;
      throw;
    }
  }
};
