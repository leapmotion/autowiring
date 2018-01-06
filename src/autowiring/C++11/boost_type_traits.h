// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/type_traits/conditional.hpp>
#include <autoboost/type_traits/decay.hpp>
#include <autoboost/type_traits/has_trivial_constructor.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/type_traits/is_abstract.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/is_base_of.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/is_floating_point.hpp>
#include <autoboost/type_traits/is_polymorphic.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/remove_extent.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/type_traits/remove_pointer.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/utility/declval.hpp>

namespace std {
  using autoboost::conditional;
  using autoboost::decay;
  using autoboost::declval;
  using autoboost::false_type;
  using autoboost::has_trivial_constructor;
  using autoboost::integral_constant;
  using autoboost::is_abstract;
  using autoboost::is_array;
  using autoboost::is_class;
  using autoboost::is_base_of;
  using autoboost::is_const;
  using autoboost::is_floating_point;
  using autoboost::is_integral;
  using autoboost::is_polymorphic;
  using autoboost::is_same;
  using autoboost::is_scalar;
  using autoboost::remove_extent;
  using autoboost::remove_reference;
  using autoboost::true_type;
  using autoboost::is_void;
  using autoboost::is_convertible;
  using autoboost::remove_cv;
  using autoboost::remove_pointer;
  using autoboost::is_reference;

  template<bool _Test, class _Type = void>
  struct enable_if {};

  template<class _Type>
  struct enable_if<true, _Type>
  {
    typedef _Type type;
  };

  template<class T, class... Args>
  struct is_constructible {
    template<class U>
    static std::true_type select(decltype(U(*static_cast<typename remove_reference<Args>::type*>(nullptr)...))*);
    
    template<class U>
    static std::false_type select(...);

    static const bool value = decltype(select<T>(static_cast<T*>(nullptr)))::value;
  };
}
