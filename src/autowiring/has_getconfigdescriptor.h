// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include RVALUE_HEADER

namespace autowiring {
  struct config_descriptor;

  /// <summary>
  /// Utility helper structure for types which have a factory New routine
  /// </summary>
  /// <remarks>
  /// A factory New routine is malformed when the return type is not implicitly castable to type T
  /// </remarks>
  template<class T, class Selector>
  struct has_well_formed_getconfigdescriptor {
    static const bool value =
      std::is_convertible<
        typename std::result_of<decltype(T::GetConfigDescriptor)&()>::type,
        autowiring::config_descriptor
      >::value &&
      !std::is_member_function_pointer<decltype(&T::GetConfigDescriptor)>::value;
  };

  template<class T>
  struct has_well_formed_getconfigdescriptor<T, std::false_type> {
    static const bool value = false;
  };

  template<typename T>
  struct has_getconfigdescriptor
  {
    template<class U>
    static std::true_type select(decltype(U::GetConfigDescriptor)*);

    template<class U>
    static std::false_type select(...);

    static const bool value = has_well_formed_getconfigdescriptor<T, decltype(select<T>(nullptr))>::value;
  };
}
