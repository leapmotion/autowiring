// Copyright Kevlin Henney, 2000-2005.
// Copyright Alexander Nasonov, 2006-2010.
// Copyright Antony Polukhin, 2011-2014.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// what:  lexical_cast custom keyword cast
// who:   contributed by Kevlin Henney,
//        enhanced with contributions from Terje Slettebo,
//        with additional fixes and suggestions from Gennaro Prota,
//        Beman Dawes, Dave Abrahams, Daryle Walker, Peter Dimov,
//        Alexander Nasonov, Antony Polukhin, Justin Viiret, Michael Hofmann,
//        Cheng Yang, Matthew Bradbury, David W. Birdsall, Pavel Korzh and other Boosters
// when:  November 2000, March 2003, June 2005, June 2006, March 2011 - 2014

#ifndef AUTOBOOST_LEXICAL_CAST_INCLUDED
#define AUTOBOOST_LEXICAL_CAST_INCLUDED

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#if defined(AUTOBOOST_NO_STRINGSTREAM) || defined(AUTOBOOST_NO_STD_WSTRING)
#define AUTOBOOST_LCAST_NO_WCHAR_T
#endif

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/lexical_cast/bad_lexical_cast.hpp>
#include <autoboost/lexical_cast/try_lexical_convert.hpp>

namespace autoboost
{
    template <typename Target, typename Source>
    inline Target lexical_cast(const Source &arg)
    {
        Target result;

        if (!autoboost::conversion::detail::try_lexical_convert(arg, result)) {
            autoboost::conversion::detail::throw_bad_cast<Source, Target>();
        }

        return result;
    }

    template <typename Target>
    inline Target lexical_cast(const char* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const char*>(chars, chars + count)
        );
    }

    template <typename Target>
    inline Target lexical_cast(const unsigned char* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const unsigned char*>(chars, chars + count)
        );
    }

    template <typename Target>
    inline Target lexical_cast(const signed char* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const signed char*>(chars, chars + count)
        );
    }

#ifndef AUTOBOOST_LCAST_NO_WCHAR_T
    template <typename Target>
    inline Target lexical_cast(const wchar_t* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const wchar_t*>(chars, chars + count)
        );
    }
#endif
#ifndef AUTOBOOST_NO_CXX11_CHAR16_T
    template <typename Target>
    inline Target lexical_cast(const char16_t* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const char16_t*>(chars, chars + count)
        );
    }
#endif
#ifndef AUTOBOOST_NO_CXX11_CHAR32_T
    template <typename Target>
    inline Target lexical_cast(const char32_t* chars, std::size_t count)
    {
        return ::autoboost::lexical_cast<Target>(
            ::autoboost::iterator_range<const char32_t*>(chars, chars + count)
        );
    }
#endif

} // namespace autoboost

#undef AUTOBOOST_LCAST_NO_WCHAR_T

#endif // AUTOBOOST_LEXICAL_CAST_INCLUDED

