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
