// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <exception>

class autowiring_error:
  public std::exception
{
public:
  autowiring_error(const char* what):
    m_what(what)
  {}

private:
  const char* m_what;

public:
  const char* what(void) const throw() override {return m_what;}
};

