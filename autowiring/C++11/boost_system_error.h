// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace std {
  using AUTOWIRING_BOOST_NAME::system::error_category;
  using AUTOWIRING_BOOST_NAME::system::generic_category;
  using AUTOWIRING_BOOST_NAME::system::system_error;

  using AUTOWIRING_BOOST_NAME::system::is_error_code_enum;

  namespace errc {
    using AUTOWIRING_BOOST_NAME::system::errc::make_error_code;
  }
}
