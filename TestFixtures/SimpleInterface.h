#pragma once

class SimpleInterface {
public:
  SimpleInterface(void):
    m_called(false)
  {}

  bool m_called;

  virtual void Method(void) = 0;
};