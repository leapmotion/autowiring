//  autoboost/filesystem/detail/macro_value.hpp  -------------------------------------------//

//  (C) Copyright John Maddock 2001 - 2003
//  (C) Copyright Jens Maurer 2001
//  (C) Copyright Peter Dimov 2001
//  (C) Copyright Darin Adler 2001
//  (C) Copyright Beman Dawes 2002

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef AUTOBOOST_FILESYSTEM_MACRO_VALUE_HPP
#define AUTOBOOST_FILESYSTEM_MACRO_VALUE_HPP

#include <autoboost/config.hpp>
#include <autoboost/assert.hpp>
#include <cstdlib>

namespace autoboost
{
  namespace detail
  {
    inline const char* macro_value(const char* name, const char* value)
    {
      static const char* no_value = "[no value]";
      static const char* not_defined = "[not defined]";

      AUTOBOOST_ASSERT_MSG(name, "name argument must not be a null pointer");
      AUTOBOOST_ASSERT_MSG(value, "value argument must not be a null pointer");

      return strcmp(name, value + 1)
        ? ((*value && *(value+1)) ? (value+1) : no_value)
        : not_defined; // name == value+1 so the macro is not defined
    }
  }  // detail
}  // autoboost

#define AUTOBOOST_MACRO_VALUE(X) autoboost::detail::macro_value(#X, AUTOBOOST_STRINGIZE(=X))

#endif  // AUTOBOOST_FILESYSTEM_MACRO_VALUE_HPP
