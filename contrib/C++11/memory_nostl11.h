// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "C++11/boost_shared_ptr.h"
#include "C++11/unique_ptr.h"
#include "C++11/make_unique.h"

#include STL_UNORDERED_SET
template<typename T>
struct std::hash<std::shared_ptr<T>>
: public std::hash<T*>
{
  std::size_t
  operator()(const std::shared_ptr<T>& __p) const
  { return std::hash<T*>::operator()(__p.get()); }
};
