//  (C) Copyright 2008-10 Anthony Williams
//  (C) Copyright 2011-2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_FUTURE_ERROR_CODE_HPP
#define AUTOBOOST_THREAD_FUTURE_ERROR_CODE_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/core/scoped_enum.hpp>
#include <autoboost/system/error_code.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

namespace autoboost
{

  //enum class future_errc
  AUTOBOOST_SCOPED_ENUM_DECLARE_BEGIN(future_errc)
  {
      broken_promise = 1,
      future_already_retrieved,
      promise_already_satisfied,
      no_state
  }
  AUTOBOOST_SCOPED_ENUM_DECLARE_END(future_errc)

  namespace system
  {
    template <>
    struct AUTOBOOST_SYMBOL_VISIBLE is_error_code_enum< ::autoboost::future_errc> : public true_type {};

    #ifdef AUTOBOOST_NO_CXX11_SCOPED_ENUMS
    template <>
    struct AUTOBOOST_SYMBOL_VISIBLE is_error_code_enum< ::autoboost::future_errc::enum_type> : public true_type { };
    #endif
  } // system

  AUTOBOOST_THREAD_DECL
  const system::error_category& future_category() AUTOBOOST_NOEXCEPT;

  namespace system
  {
    inline
    error_code
    make_error_code(future_errc e) AUTOBOOST_NOEXCEPT
    {
        return error_code(underlying_cast<int>(e), autoboost::future_category());
    }

    inline
    error_condition
    make_error_condition(future_errc e) AUTOBOOST_NOEXCEPT
    {
        return error_condition(underlying_cast<int>(e), autoboost::future_category());
    }
  } // system
} // autoboost

#endif // header
