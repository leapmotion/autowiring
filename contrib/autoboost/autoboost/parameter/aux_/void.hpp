// Copyright Daniel Wallin, David Abrahams 2005. Use, modification and
// distribution is subject to the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PARAMETER_VOID_050329_HPP
#define AUTOBOOST_PARAMETER_VOID_050329_HPP

namespace autoboost { namespace parameter {

// A placemarker for "no argument passed."
// MAINTAINER NOTE: Do not make this into a metafunction
struct void_ {};

namespace aux
{

  inline void_& void_reference()
  {
      static void_ instance;
      return instance;
  }

} // namespace aux

}} // namespace autoboost::parameter

#endif // AUTOBOOST_PARAMETER_VOID_050329_HPP

