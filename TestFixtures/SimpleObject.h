// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _SIMPLEOBJECT_H
#define _SIMPLEOBJECT_H

// Simple ContextMember class:
class SimpleObject:
  public ContextMember
{
public:
  SimpleObject(void):
    zero(0),
    one(1)
  {}

  ~SimpleObject(void) {
  }

  int zero;
  int one;
};

#endif
