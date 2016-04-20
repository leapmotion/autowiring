/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         pattern_except.hpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Declares pattern-matching exception classes.
  */

#ifndef AUTOBOOST_RE_PAT_EXCEPT_HPP
#define AUTOBOOST_RE_PAT_EXCEPT_HPP

#ifndef AUTOBOOST_REGEX_CONFIG_HPP
#include <autoboost/regex/config.hpp>
#endif

#include <stdexcept>
#include <cstddef>
#include <autoboost/regex/v4/error_type.hpp>

namespace autoboost{

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#endif
class AUTOBOOST_REGEX_DECL regex_error : public std::runtime_error
{
public:
   explicit regex_error(const std::string& s, regex_constants::error_type err = regex_constants::error_unknown, std::ptrdiff_t pos = 0);
   explicit regex_error(regex_constants::error_type err);
   ~regex_error() throw();
   regex_constants::error_type code()const
   { return m_error_code; }
   std::ptrdiff_t position()const
   { return m_position; }
   void raise()const;
private:
   regex_constants::error_type m_error_code;
   std::ptrdiff_t m_position;
};

typedef regex_error bad_pattern;
typedef regex_error bad_expression;

namespace re_detail{

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL raise_runtime_error(const std::runtime_error& ex);

template <class traits>
void raise_error(const traits& t, regex_constants::error_type code)
{
   (void)t;  // warning suppression
   std::runtime_error e(t.error_string(code));
   ::autoboost::re_detail::raise_runtime_error(e);
}

}

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4103)
#endif
#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

} // namespace autoboost

#endif



