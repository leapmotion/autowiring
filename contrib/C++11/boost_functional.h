// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <boost/function.hpp>
#include <boost/detail/is_sorted.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/utility/result_of.hpp>

namespace std {
  using boost::function;
  using boost::ref;
  using boost::detail::is_sorted;
  using boost::bind;
  using boost::result_of;

  namespace placeholders {
    boost::lambda::placeholder1_type _1;
    boost::lambda::placeholder2_type _2;
    boost::lambda::placeholder3_type _3;
    boost::lambda::placeholder4_type _4;
  }
}
