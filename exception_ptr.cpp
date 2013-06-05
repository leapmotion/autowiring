#include "stdafx.h"
#include "exception_ptr.h"

using namespace std;

exception_ptr::exception_ptr(std::exception& except):
  m_except(except)
{
}

exception_ptr::~exception_ptr(void) {
}

void exception_ptr::_RethrowException(void) const {
  throw m_except;
}
