// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _BOOST_EXCEPTION_PTR_H
#define _BOOST_EXCEPTION_PTR_H

#include <boost/exception_ptr.hpp>
namespace std {
  using boost::exception_ptr;
  using boost::rethrow_exception;
  using boost::current_exception;
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
#endif
