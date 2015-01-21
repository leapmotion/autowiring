// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/system/error_code.hpp>
#include <autoboost/system/system_error.hpp>

namespace std {
  using autoboost::system::error_category;
  using autoboost::system::generic_category;
  using autoboost::system::system_error;

  using autoboost::system::is_error_code_enum;

  namespace errc {
    using autoboost::system::errc::make_error_code;
  }
}
