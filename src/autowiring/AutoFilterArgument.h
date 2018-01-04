// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include <typeinfo>

namespace autowiring {
template<class T>
class auto_arg;

/// <summary>
/// AutoFilter argument disposition
/// </summary>
struct AutoFilterArgument {
  AutoFilterArgument(void) = default;

protected:
  AutoFilterArgument(
    bool is_input,
    bool is_output,
    bool is_rvalue,
    bool is_shared,
    bool is_multi,
    auto_id id,
    int tshift
  ) :
    is_input(is_input),
    is_output(is_output),
    is_rvalue(is_rvalue),
    is_shared(is_shared),
    is_multi(is_multi),
    id(id),
    tshift(tshift)
  {}

public:
  const bool is_input = false;
  const bool is_output = false;
  const bool is_rvalue = false;
  const bool is_shared = false;
  const bool is_multi = false;
  const auto_id id = auto_id_t<void>{};
  const int tshift = 0;

  explicit operator bool(void) const {
    return static_cast<bool>(id);
  }
};

template<typename T>
struct AutoFilterArgumentT:
  AutoFilterArgument
{
  AutoFilterArgumentT(void) :
    AutoFilterArgument(
      auto_arg<T>::is_input,
      auto_arg<T>::is_output,
      auto_arg<T>::is_rvalue,
      auto_arg<T>::is_shared,
      auto_arg<T>::is_multi,
      typename auto_arg<T>::id_type(),
      auto_arg<T>::tshift
    )
  {}
};

}
