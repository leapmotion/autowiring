// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
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
  }
}
