// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/remove_extent.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>

namespace std {
  using AUTOWIRING_BOOST_NAME::conditional;
  using AUTOWIRING_BOOST_NAME::decay;
  using AUTOWIRING_BOOST_NAME::false_type;
  using AUTOWIRING_BOOST_NAME::has_trivial_constructor;
  using AUTOWIRING_BOOST_NAME::integral_constant;
  using AUTOWIRING_BOOST_NAME::is_abstract;
  using AUTOWIRING_BOOST_NAME::is_array;
  using AUTOWIRING_BOOST_NAME::is_class;
  using AUTOWIRING_BOOST_NAME::is_base_of;
  using AUTOWIRING_BOOST_NAME::is_const;
  using AUTOWIRING_BOOST_NAME::is_floating_point;
  using AUTOWIRING_BOOST_NAME::is_integral;
  using AUTOWIRING_BOOST_NAME::is_polymorphic;
  using AUTOWIRING_BOOST_NAME::is_same;
  using AUTOWIRING_BOOST_NAME::is_scalar;
  using AUTOWIRING_BOOST_NAME::remove_extent;
  using AUTOWIRING_BOOST_NAME::remove_reference;
  using AUTOWIRING_BOOST_NAME::true_type;
  using AUTOWIRING_BOOST_NAME::is_void;
  using AUTOWIRING_BOOST_NAME::is_convertible;

  template<bool _Test, class _Type = void>
  struct enable_if {};

  template<class _Type>
  struct enable_if<true, _Type>
  {
    typedef _Type type;
  };
}
