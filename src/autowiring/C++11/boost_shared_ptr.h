// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

// We need to use the autoboost version of shared_ptr everywhere
// if it isn't provided by the framework
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include <autoboost/smart_ptr/make_shared.hpp>
#include <autoboost/smart_ptr/shared_ptr.hpp>
#include <autoboost/smart_ptr/weak_ptr.hpp>
#include <autoboost/smart_ptr/enable_shared_from_this.hpp>
#include <autoboost/smart_ptr/owner_less.hpp>
#pragma GCC diagnostic pop
#include <autoboost/asio/detail/shared_ptr.hpp>

namespace std {
  using autoboost::shared_ptr;
  using autoboost::weak_ptr;
  using autoboost::enable_shared_from_this;
  using autoboost::owner_less;

  using autoboost::const_pointer_cast;
  using autoboost::static_pointer_cast;
  using autoboost::dynamic_pointer_cast;
  using autoboost::make_shared;
}
