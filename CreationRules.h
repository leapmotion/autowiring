#pragma once
#include "ContextMember.h"
#include "has_simple_constructor.h"
#include "has_static_new.h"
#include "SlotInformation.h"
#include TYPE_TRAITS_HEADER
#include RVALUE_HEADER
#include <new>


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
    auto* pSpace = Allocate<U>(nullptr);

    try {
      // Stack location and placement new in one expression
      return
        SlotInformationStackLocation::PushStackLocation<U>(reinterpret_cast<U*>(pSpace)),
        ::new (pSpace) U(std::forward<Args>(args)...);
    }
    catch(...) {
      // Don't want memory leaks--but we also want to avoid calling the destructor, here, so we cast to void before freeing
      Free<U>(pSpace, nullptr);
      throw;
    }
  }

  template<void* (*)(size_t)>
  struct alloc_fn {};

  template<typename U>
  static void* Allocate(alloc_fn<&U::operator new>*) {
    return U::operator new(sizeof(U));
  }

  template<typename U>
  static void* Allocate(...) {
    return ::operator new(sizeof(U));
  }

  template<void(*)(void*)>
  struct free_fn {};

  template<typename U>
  static void Free(void* ptr, free_fn<&U::operator delete>*) {
    U::operator delete(ptr);
  }

  template<typename U>
  static void Free(void* ptr, ...) {
    ::operator delete(ptr);
  }
};
