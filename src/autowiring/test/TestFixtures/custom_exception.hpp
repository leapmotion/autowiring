// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include <exception>

class custom_exception:
  public std::exception
{
public:
  custom_exception(int value) :
    m_value(value)
  {
  }

  int m_value;
};
