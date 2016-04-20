/*
 *
 * Copyright (c) 1998-2009
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex_format.hpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Provides formatting output routines for search and replace
  *                operations.  Note this is an internal header file included
  *                by regex.hpp, do not include on its own.
  */

#ifndef AUTOBOOST_REGEX_V4_REGEX_REPLACE_HPP
#define AUTOBOOST_REGEX_V4_REGEX_REPLACE_HPP


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

template <class OutputIterator, class BidirectionalIterator, class traits, class charT, class Formatter>
OutputIterator regex_replace(OutputIterator out,
                         BidirectionalIterator first,
                         BidirectionalIterator last,
                         const basic_regex<charT, traits>& e,
                         Formatter fmt,
                         match_flag_type flags = match_default)
{
   regex_iterator<BidirectionalIterator, charT, traits> i(first, last, e, flags);
   regex_iterator<BidirectionalIterator, charT, traits> j;
   if(i == j)
   {
      if(!(flags & regex_constants::format_no_copy))
         out = re_detail::copy(first, last, out);
   }
   else
   {
      BidirectionalIterator last_m(first);
      while(i != j)
      {
         if(!(flags & regex_constants::format_no_copy))
            out = re_detail::copy(i->prefix().first, i->prefix().second, out);
         out = i->format(out, fmt, flags, e);
         last_m = (*i)[0].second;
         if(flags & regex_constants::format_first_only)
            break;
         ++i;
      }
      if(!(flags & regex_constants::format_no_copy))
         out = re_detail::copy(last_m, last, out);
   }
   return out;
}

template <class traits, class charT, class Formatter>
std::basic_string<charT> regex_replace(const std::basic_string<charT>& s,
                         const basic_regex<charT, traits>& e,
                         Formatter fmt,
                         match_flag_type flags = match_default)
{
   std::basic_string<charT> result;
   re_detail::string_out_iterator<std::basic_string<charT> > i(result);
   regex_replace(i, s.begin(), s.end(), e, fmt, flags);
   return result;
}

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

#endif  // AUTOBOOST_REGEX_V4_REGEX_REPLACE_HPP


