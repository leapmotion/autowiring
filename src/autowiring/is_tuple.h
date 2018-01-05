// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
namespace autowiring {

template<typename T>
struct is_tuple {
  static const bool value = false;
};

template<typename... Args>
struct is_tuple<std::tuple<Args...>> {
  static const bool value = true;
};

}
