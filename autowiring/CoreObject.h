// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// General object base, used to make conversions possible between various shared pointer implementations.
/// Withouat an object base, there is no way to do generic autowiring.
/// </summary>
class CoreObject {
public:
  CoreObject(void);
  virtual ~CoreObject(void);
};

// Temporarily typedef to old name of 'CoreObject' until all users of Autowiring have been updated
typedef CoreObject Object;
