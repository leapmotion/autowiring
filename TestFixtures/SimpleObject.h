#pragma once

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

