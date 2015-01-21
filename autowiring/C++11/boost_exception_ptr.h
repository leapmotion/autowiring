// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/exception_ptr.hpp>
namespace std {
  using autoboost::exception_ptr;
  using autoboost::rethrow_exception;
  using autoboost::current_exception;
}

class throw_exception_util {
public:
  template<class T>
  inline throw_exception_util(T&& type) {
    BOOST_THROW_EXCEPTION(type);
  }
};

// Use cast constructor to have throw_exception use BOOST_THROW_EXCEPTION to throw the exception
#define throw_rethrowable (throw_exception_util)
