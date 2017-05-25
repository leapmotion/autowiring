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

#ifndef AUTOBOOST_LEXICAL_CAST_LEXICAL_CAST_OLD_HPP
#define AUTOBOOST_LEXICAL_CAST_LEXICAL_CAST_OLD_HPP

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#if defined(AUTOBOOST_NO_STRINGSTREAM) || defined(AUTOBOOST_NO_STD_WSTRING)
#define AUTOBOOST_LCAST_NO_WCHAR_T
#endif

#include <climits>
#include <cstddef>
#include <string>
#include <cstring>
#include <cstdio>
#include <autoboost/limits.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/detail/lcast_precision.hpp>
#include <autoboost/detail/workaround.hpp>

#ifdef AUTOBOOST_NO_STRINGSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include <autoboost/lexical_cast/bad_lexical_cast.hpp>
#include <autoboost/lexical_cast/detail/widest_char.hpp>

namespace autoboost {
    namespace detail
    {

        // selectors for choosing stream character type
        template<typename Type>
        struct stream_char
        {
            typedef char type;
        };

#ifndef AUTOBOOST_LCAST_NO_WCHAR_T
#ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
        template<>
        struct stream_char<wchar_t>
        {
            typedef wchar_t type;
        };
#endif

        template<>
        struct stream_char<wchar_t *>
        {
            typedef wchar_t type;
        };

        template<>
        struct stream_char<const wchar_t *>
        {
            typedef wchar_t type;
        };

        template<>
        struct stream_char<std::wstring>
        {
            typedef wchar_t type;
        };
#endif

        // stream wrapper for handling lexical conversions
        template<typename Target, typename Source, typename Traits>
        class lexical_stream
        {
        private:
            typedef typename widest_char<
                typename stream_char<Target>::type,
                typename stream_char<Source>::type>::type char_type;

            typedef Traits traits_type;

        public:
            lexical_stream(char_type* = 0, char_type* = 0)
            {
                stream.unsetf(std::ios::skipws);
                lcast_set_precision(stream, static_cast<Source*>(0), static_cast<Target*>(0) );
            }
            ~lexical_stream()
            {
                #if defined(AUTOBOOST_NO_STRINGSTREAM)
                stream.freeze(false);
                #endif
            }
            bool operator<<(const Source &input)
            {
                return !(stream << input).fail();
            }
            template<typename InputStreamable>
            bool operator>>(InputStreamable &output)
            {
                return !is_pointer<InputStreamable>::value &&
                       stream >> output &&
                       stream.get() == traits_type::eof();
            }

            bool operator>>(std::string &output)
            {
                #if defined(AUTOBOOST_NO_STRINGSTREAM)
                stream << '\0';
                #endif
                stream.str().swap(output);
                return true;
            }
            #ifndef AUTOBOOST_LCAST_NO_WCHAR_T
            bool operator>>(std::wstring &output)
            {
                stream.str().swap(output);
                return true;
            }
            #endif

        private:
            #if defined(AUTOBOOST_NO_STRINGSTREAM)
            std::strstream stream;
            #elif defined(AUTOBOOST_NO_STD_LOCALE)
            std::stringstream stream;
            #else
            std::basic_stringstream<char_type,traits_type> stream;
            #endif
        };
    }

    // call-by-value fallback version (deprecated)

    template<typename Target, typename Source>
    Target lexical_cast(Source arg)
    {
        typedef typename detail::widest_char<
            AUTOBOOST_DEDUCED_TYPENAME detail::stream_char<Target>::type
          , AUTOBOOST_DEDUCED_TYPENAME detail::stream_char<Source>::type
        >::type char_type;

        typedef std::char_traits<char_type> traits;
        detail::lexical_stream<Target, Source, traits> interpreter;
        Target result;

        if(!(interpreter << arg && interpreter >> result))
            autoboost::conversion::detail::throw_bad_cast<Source, Target>();
        return result;
    }

} // namespace autoboost

#undef AUTOBOOST_LCAST_NO_WCHAR_T

#endif // AUTOBOOST_LEXICAL_CAST_LEXICAL_CAST_OLD_HPP

