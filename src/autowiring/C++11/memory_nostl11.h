// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

#include "boost_shared_ptr.h"
#include "unique_ptr.h"
#include "make_unique.h"
#include STL_UNORDERED_SET

template<typename T>
struct std::hash<std::shared_ptr<T>>:
  public std::hash<T*>
{
  std::size_t operator()(const std::shared_ptr<T>& __p) const
  {
    return std::hash<T*>::operator()(__p.get());
  }
};

namespace std {
  using autoboost::enable_shared_from_this;
}
