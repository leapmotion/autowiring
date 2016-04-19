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
  *   FILE         regex.cpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Declares autoboost::basic_regex<> and associated
  *                functions and classes. This header is the main
  *                entry point for the template regex code.
  */

#ifndef AUTOBOOST_RE_REGEX_HPP_INCLUDED
#define AUTOBOOST_RE_REGEX_HPP_INCLUDED

#ifdef __cplusplus

// what follows is all C++ don't include in C builds!!

#ifndef AUTOBOOST_REGEX_CONFIG_HPP
#include <autoboost/regex/config.hpp>
#endif
#ifndef AUTOBOOST_REGEX_WORKAROUND_HPP
#include <autoboost/regex/v4/regex_workaround.hpp>
#endif

#ifndef AUTOBOOST_REGEX_FWD_HPP
#include <autoboost/regex_fwd.hpp>
#endif
#ifndef AUTOBOOST_REGEX_TRAITS_HPP
#include <autoboost/regex/regex_traits.hpp>
#endif
#ifndef AUTOBOOST_REGEX_RAW_BUFFER_HPP
#include <autoboost/regex/v4/error_type.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_MATCH_FLAGS
#include <autoboost/regex/v4/match_flags.hpp>
#endif
#ifndef AUTOBOOST_REGEX_RAW_BUFFER_HPP
#include <autoboost/regex/v4/regex_raw_buffer.hpp>
#endif
#ifndef AUTOBOOST_RE_PAT_EXCEPT_HPP
#include <autoboost/regex/pattern_except.hpp>
#endif

#ifndef AUTOBOOST_REGEX_V4_CHAR_REGEX_TRAITS_HPP
#include <autoboost/regex/v4/char_regex_traits.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_STATES_HPP
#include <autoboost/regex/v4/states.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_REGBASE_HPP
#include <autoboost/regex/v4/regbase.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_ITERATOR_TRAITS_HPP
#include <autoboost/regex/v4/iterator_traits.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_BASIC_REGEX_HPP
#include <autoboost/regex/v4/basic_regex.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_BASIC_REGEX_CREATOR_HPP
#include <autoboost/regex/v4/basic_regex_creator.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_BASIC_REGEX_PARSER_HPP
#include <autoboost/regex/v4/basic_regex_parser.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_SUB_MATCH_HPP
#include <autoboost/regex/v4/sub_match.hpp>
#endif
#ifndef AUTOBOOST_REGEX_FORMAT_HPP
#include <autoboost/regex/v4/regex_format.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_MATCH_RESULTS_HPP
#include <autoboost/regex/v4/match_results.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_PROTECTED_CALL_HPP
#include <autoboost/regex/v4/protected_call.hpp>
#endif
#ifndef AUTOBOOST_REGEX_MATCHER_HPP
#include <autoboost/regex/v4/perl_matcher.hpp>
#endif
//
// template instances:
//
#define AUTOBOOST_REGEX_CHAR_T char
#ifdef AUTOBOOST_REGEX_NARROW_INSTANTIATE
#  define AUTOBOOST_REGEX_INSTANTIATE
#endif
#include <autoboost/regex/v4/instances.hpp>
#undef AUTOBOOST_REGEX_CHAR_T
#ifdef AUTOBOOST_REGEX_INSTANTIATE
#  undef AUTOBOOST_REGEX_INSTANTIATE
#endif

#ifndef AUTOBOOST_NO_WREGEX
#define AUTOBOOST_REGEX_CHAR_T wchar_t
#ifdef AUTOBOOST_REGEX_WIDE_INSTANTIATE
#  define AUTOBOOST_REGEX_INSTANTIATE
#endif
#include <autoboost/regex/v4/instances.hpp>
#undef AUTOBOOST_REGEX_CHAR_T
#ifdef AUTOBOOST_REGEX_INSTANTIATE
#  undef AUTOBOOST_REGEX_INSTANTIATE
#endif
#endif

#if !defined(AUTOBOOST_NO_WREGEX) && defined(AUTOBOOST_REGEX_HAS_OTHER_WCHAR_T)
#define AUTOBOOST_REGEX_CHAR_T unsigned short
#ifdef AUTOBOOST_REGEX_US_INSTANTIATE
#  define AUTOBOOST_REGEX_INSTANTIATE
#endif
#include <autoboost/regex/v4/instances.hpp>
#undef AUTOBOOST_REGEX_CHAR_T
#ifdef AUTOBOOST_REGEX_INSTANTIATE
#  undef AUTOBOOST_REGEX_INSTANTIATE
#endif
#endif


namespace autoboost{
#ifdef AUTOBOOST_REGEX_NO_FWD
typedef basic_regex<char, regex_traits<char> > regex;
#ifndef AUTOBOOST_NO_WREGEX
typedef basic_regex<wchar_t, regex_traits<wchar_t> > wregex;
#endif
#endif

typedef match_results<const char*> cmatch;
typedef match_results<std::string::const_iterator> smatch;
#ifndef AUTOBOOST_NO_WREGEX
typedef match_results<const wchar_t*> wcmatch;
typedef match_results<std::wstring::const_iterator> wsmatch;
#endif

} // namespace autoboost
#ifndef AUTOBOOST_REGEX_MATCH_HPP
#include <autoboost/regex/v4/regex_match.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_REGEX_SEARCH_HPP
#include <autoboost/regex/v4/regex_search.hpp>
#endif
#ifndef AUTOBOOST_REGEX_ITERATOR_HPP
#include <autoboost/regex/v4/regex_iterator.hpp>
#endif
#ifndef AUTOBOOST_REGEX_TOKEN_ITERATOR_HPP
#include <autoboost/regex/v4/regex_token_iterator.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_REGEX_GREP_HPP
#include <autoboost/regex/v4/regex_grep.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_REGEX_REPLACE_HPP
#include <autoboost/regex/v4/regex_replace.hpp>
#endif
#ifndef AUTOBOOST_REGEX_V4_REGEX_MERGE_HPP
#include <autoboost/regex/v4/regex_merge.hpp>
#endif
#ifndef AUTOBOOST_REGEX_SPLIT_HPP
#include <autoboost/regex/v4/regex_split.hpp>
#endif

#endif  // __cplusplus

#endif  // include































