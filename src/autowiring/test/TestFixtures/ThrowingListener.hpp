#pragma once

class ThrowingListener {
public:
  ThrowingListener(void) {}
  virtual void DoThrow(void) = 0;
};
