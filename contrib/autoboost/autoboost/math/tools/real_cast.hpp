//  Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TOOLS_REAL_CAST_HPP
#define AUTOBOOST_MATH_TOOLS_REAL_CAST_HPP

#include <autoboost/math/tools/config.hpp>

#ifdef _MSC_VER
#pragma once
#endif

namespace autoboost{ namespace math
{
  namespace tools
  {
    template <class To, class T>
    inline AUTOBOOST_MATH_CONSTEXPR To real_cast(T t) AUTOBOOST_NOEXCEPT_IF(AUTOBOOST_MATH_IS_FLOAT(T) && AUTOBOOST_MATH_IS_FLOAT(To))
    {
       return static_cast<To>(t);
    }
  } // namespace tools
} // namespace math
} // namespace autoboost

#endif // AUTOBOOST_MATH_TOOLS_REAL_CAST_HPP



