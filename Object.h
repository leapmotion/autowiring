// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _OBJECT_H
#define _OBJECT_H

/// <summary>
/// General object base, used to make conversions possible between various shared pointer implementations.
/// Withouat an object base, there is no way to do generic autowiring.
/// </summary>
class Object {
public:
  // We are our own ground type
  typedef Object ground;

  virtual ~Object(void) {};
};

#endif
