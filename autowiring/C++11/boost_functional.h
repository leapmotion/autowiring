// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <boost/function.hpp>
#include <boost/detail/is_sorted.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/utility/result_of.hpp>

namespace std {
  using AUTOWIRING_BOOST_NAME::function;
  using AUTOWIRING_BOOST_NAME::ref;
  using AUTOWIRING_BOOST_NAME::detail::is_sorted;
  using AUTOWIRING_BOOST_NAME::bind;
  using AUTOWIRING_BOOST_NAME::result_of;

  namespace placeholders {
    AUTOWIRING_BOOST_NAME::lambda::placeholder1_type _1;
    AUTOWIRING_BOOST_NAME::lambda::placeholder2_type _2;
    AUTOWIRING_BOOST_NAME::lambda::placeholder3_type _3;
  }
}
