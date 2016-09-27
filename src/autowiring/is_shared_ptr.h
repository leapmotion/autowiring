// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {

template<class T>
struct is_shared_ptr:
  public std::false_type
{};

template<class T>
struct is_shared_ptr<T&> :
  is_shared_ptr<T>
{};

template<class T>
struct is_shared_ptr<const T> :
  is_shared_ptr<T>
{};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> :
  public std::true_type
{
  typedef T type;
};

}
