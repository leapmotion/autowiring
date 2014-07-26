// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

// We need to use the boost version of shared_ptr everywhere
// if it isn't provided by the framework
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#pragma GCC diagnostic pop
#include <boost/asio/detail/shared_ptr.hpp>

namespace std {
  using boost::shared_ptr;
  using boost::weak_ptr;
  using boost::enable_shared_from_this;
  using boost::owner_less;

  using boost::const_pointer_cast;
  using boost::static_pointer_cast;
  using boost::dynamic_pointer_cast;
  using boost::make_shared;
}
