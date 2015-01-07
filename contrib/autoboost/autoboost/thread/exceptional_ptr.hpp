// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_EXCEPTIONAL_PTR_HPP
#define AUTOBOOST_THREAD_EXCEPTIONAL_PTR_HPP

#include <autoboost/thread/detail/move.hpp>
#include <autoboost/exception_ptr.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
  struct exceptional_ptr {
    exception_ptr ptr_;

    exceptional_ptr() : ptr_() {}
    explicit exceptional_ptr(exception_ptr ex) : ptr_(ex) {}
    template <class E>
    explicit exceptional_ptr(AUTOBOOST_FWD_REF(E) ex) : ptr_(autoboost::copy_exception(autoboost::forward<E>(ex))) {}
  };

  template <class E>
  inline exceptional_ptr make_exceptional(AUTOBOOST_FWD_REF(E) ex) {
    return exceptional_ptr(autoboost::forward<E>(ex));
  }

  inline exceptional_ptr make_exceptional(exception_ptr ex)
  {
    return exceptional_ptr(ex);
  }

  inline exceptional_ptr make_exceptional()
  {
    return exceptional_ptr();
  }

} // namespace autoboost

#include <autoboost/config/abi_suffix.hpp>

#endif
