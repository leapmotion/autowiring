// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

#include TYPE_TRAITS_HEADER
#include <typeinfo>
#include <tr1/functional_hash.h>

namespace std {

class type_index {
public:
  type_index(const type_info& info):
    _info(&info)
  {}

  bool operator==(const type_index& rhs) const {
    return *_info == *rhs._info;
  }

  bool operator<(const type_index& rhs) const {
    return (_info->before(*rhs._info) != 0);
  }

  bool operator!=(const type_index& rhs) const {
    return !operator==(rhs);
  }

  size_t hash_code() const {
    return (size_t)_info;
  }

  const char* name() const {
    return _info->name();
  }

private:
  const type_info* _info;

};

namespace tr1 {
  template<>
  struct hash<std::type_index>:
  public std::unary_function<std::type_index, size_t>
  {
    size_t operator()(std::type_index val) const {
      return val.hash_code();
    }
  };
}

};
