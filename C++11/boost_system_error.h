// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace std {
  using errc = boost::system::errc::errc_t;
  using boost::system::error_category;
  using boost::system::generic_category;
  using boost::system::make_error_code;
  using boost::system::system_error;
}
