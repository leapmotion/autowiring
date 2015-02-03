// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include <typeinfo>

// Checks if an Object* listens to a event T;
struct TypeIdentifierBase {
  virtual bool IsSameAs(const CoreObject* obj) = 0;
  virtual const std::type_info& Type() = 0;
};

template<typename T>
  struct TypeIdentifier:
public TypeIdentifierBase
{
  // true if "obj" is an event receiver for T
  bool IsSameAs(const CoreObject* obj) override {
    return !!dynamic_cast<const T*>(obj);
  }
  
  const std::type_info& Type() override {
    return typeid(T);
  }
};
