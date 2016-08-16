// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "has_simple_constructor.h"
#include "has_static_new.h"
#include "SlotInformation.h"
#include RVALUE_HEADER

class CoreContext;

namespace autowiring {

template<typename T>
struct is_injectable
{
  static const bool value = has_simple_constructor<T>::value || has_static_new<T>::value;
};

enum class construction_strategy {
  // Just use new
  standard,

  // Use the static new defined by the type
  factory_new
};

template<class T, class... Args>
struct select_strategy {
  static const construction_strategy value =
    // If a factory new is defined, then use it
    has_static_new<T, typename std::remove_reference<Args>::type...>::value ?
    construction_strategy::factory_new :

    // Otherwise we give up and just try to compose the type directly
    construction_strategy::standard;
};

/// <summary>
/// Performs a platform-independent aligned allocation
/// </summary>
void* aligned_malloc(size_t ncb, size_t align);

/// <summary>
/// Corrolary of aligned_malloc
/// </summary>
void aligned_free(void* ptr);

template<construction_strategy>
struct strategy_impl {};

template<void* (*)(size_t)>
struct alloc_fn {};

template<typename U>
static U* Allocate(alloc_fn<&U::operator new>*) {
  return (U*) U::operator new(sizeof(U));
}

template<typename U>
static U* Allocate(...) {
  return (U*) ::operator new(sizeof(U));
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

/// <summary>
/// Creation rules helper type, used to select one of the creation strategies based on an arglist
/// </summary>
template<construction_strategy, class T, class... Args>
struct crh;

template<class T, class... Args>
struct crh<construction_strategy::factory_new, T, Args...>
{
  // Actual constructed type is directly the specified type
  typedef T TActual;

  static_assert(
    std::is_base_of<CoreObject, T>::value,
    "If type T provides a static New method, then the constructed type MUST directly inherit Object"
  );

  static T* New(const CoreContext&, Args&&... args) {
    auto* retVal = T::New(std::forward<Args>(args)...);
    static_assert(
      std::is_convertible<decltype(retVal), T*>::value,
      "Attempted to create T using T::New, but the type of T::New() is not derived from T"
    );
    return retVal;
  }
};

template<class T, class... Args>
struct crh<construction_strategy::standard, T, Args...> {
  // If T doesn't inherit Object, then we need to compose a unifying type which does
  typedef typename SelectTypeUnifier<T>::type TActual;

  static_assert(!has_static_new<T, Args...>::value, "Can't inject member with arguments if it has a static New");

  static TActual* New(const CoreContext&, Args&&... args) {
    // Allocate slot first before registration
    auto* pSpace = Allocate<TActual>(nullptr);

    try {
      // Push a new stack location so that all constructors from here know the injected type under construction
      SlotInformationStackLocation loc(
        SlotInformationStump<T>::s_stump,
        pSpace,
        sizeof(TActual)
      );

      // And now we create our space
      return ::new (pSpace) TActual(std::forward<Args>(args)...);
    }
    catch(...) {
      // Use our free method, which will correctly invoke the deallocator for this space
      Free<TActual>(pSpace, nullptr);
      throw;
    }
  }
};

/// <summary>
/// Simple structure to centralize knowledge about how to create types with various declarations
/// </summary>
template<class T, class... Args>
struct CreationRules :
  crh<select_strategy<T, Args...>::value, T, Args...>
{
};

}
