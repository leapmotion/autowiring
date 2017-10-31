// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autoboost/function.hpp>
#include <autoboost/detail/is_sorted.hpp>
#include <autoboost/bind.hpp>
#include <autoboost/lambda/bind.hpp>
#include <autoboost/utility/result_of.hpp>

namespace std {
  using autoboost::function;
  using autoboost::ref;
  using autoboost::detail::is_sorted;
  using autoboost::bind;
  using autoboost::result_of;

  namespace placeholders {
    autoboost::lambda::placeholder1_type _1;
    autoboost::lambda::placeholder2_type _2;
    autoboost::lambda::placeholder3_type _3;
  }
}
