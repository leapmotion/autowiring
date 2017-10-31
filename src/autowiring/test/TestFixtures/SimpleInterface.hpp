// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

class SimpleInterface {
public:
  SimpleInterface(void):
    m_called(false)
  {}

  virtual ~SimpleInterface(void) {}

  bool m_called;

  virtual void Method(void) = 0;
};
