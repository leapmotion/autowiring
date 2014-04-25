#pragma once
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

  template<typename U>
  static typename std::enable_if<!has_static_new<U>::value, U*>::type New(void) {
    static_assert(!std::is_abstract<U>::value, "Cannot create a type which is abstract");
    static_assert(has_simple_constructor<U>::value, "Attempted to create a type which did not provide a zero-arguments ctor");
    return new U;
  }
  
  //
  template<typename U, typename... Args>
  static typename std::enable_if<!has_static_new<U>::value, U*>::type New(Args&&... args) {
    static_assert(!std::is_abstract<U>::value, "Cannot create a type which is abstract");
    static_assert(sizeof...(Args) != 0, "Template specialization failed");
    static_assert(!has_static_new<U>::value, "Can't inject member with arguments if it has a static new");
    static_assert(!has_simple_constructor<U>::value, "Can't inject member with arguments if it has a default constructor");
    
    return new U(std::forward<Args>(args)...);
  }
};