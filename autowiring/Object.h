// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// General object base, used to make conversions possible between various shared pointer implementations.
/// Withouat an object base, there is no way to do generic autowiring.
/// </summary>
class Object {
public:
  Object(void);
  virtual ~Object(void);
};
