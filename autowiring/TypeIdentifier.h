// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include <typeinfo>

// Checks if an Object* listens to a event T;
struct TypeIdentifierBase {
  /// <summary>
  /// The runtime type information corresponding to this identifier
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;

  /// <returns>
  /// True if this type identifier can cast the specified CoreObject
  /// </returns>
  virtual bool IsSameAs(const CoreObject* obj) const = 0;
};
