// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/exception_ptr.hpp>
namespace std {
  using AUTOWIRING_BOOST_NAME::exception_ptr;
  using AUTOWIRING_BOOST_NAME::rethrow_exception;
  using AUTOWIRING_BOOST_NAME::current_exception;
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
