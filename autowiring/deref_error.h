// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include <typeinfo>

template<class T>
class AutowirableSlot;

template<typename T>
class AutowiredFast;

namespace autowiring {
  std::string GenerateExceptionTextAWF(const std::type_info& ti);
  std::string GenerateExceptionTextAW(const std::type_info& ti);

  class deref_error:
    public autowiring_error
  {
  public:
    template<typename T>
    deref_error(const AutowirableSlot<T>&) :
      autowiring_error(GenerateExceptionTextAW(typeid(T)))
    {};

    template<typename T>
    deref_error(const AutowiredFast<T>&) :
      autowiring_error(GenerateExceptionTextAWF(typeid(T)))
    {};
  };
}
