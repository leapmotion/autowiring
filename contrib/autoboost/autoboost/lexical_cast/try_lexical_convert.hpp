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

#ifndef AUTOBOOST_LEXICAL_CAST_TRY_LEXICAL_CONVERT_HPP
#define AUTOBOOST_LEXICAL_CAST_TRY_LEXICAL_CONVERT_HPP

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#if defined(__clang__) || (defined(__GNUC__) && \
    !(defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && \
    (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

#include <string>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/identity.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_arithmetic.hpp>

#include <autoboost/lexical_cast/detail/is_character.hpp>
#include <autoboost/lexical_cast/detail/converter_numeric.hpp>
#include <autoboost/lexical_cast/detail/converter_lexical.hpp>

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/container/container_fwd.hpp>

namespace autoboost {
    namespace detail
    {
        template<typename T>
        struct is_stdstring
            : autoboost::false_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_stdstring< std::basic_string<CharT, Traits, Alloc> >
            : autoboost::true_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_stdstring< autoboost::container::basic_string<CharT, Traits, Alloc> >
            : autoboost::true_type
        {};

        template<typename Target, typename Source>
        struct is_arithmetic_and_not_xchars
        {
            typedef autoboost::mpl::bool_<
                    !(autoboost::detail::is_character<Target>::value) &&
                    !(autoboost::detail::is_character<Source>::value) &&
                    autoboost::is_arithmetic<Source>::value &&
                    autoboost::is_arithmetic<Target>::value
                > type;

            AUTOBOOST_STATIC_CONSTANT(bool, value = (
                type::value
            ));
        };

        /*
         * is_xchar_to_xchar<Target, Source>::value is true,
         * Target and Souce are char types of the same size 1 (char, signed char, unsigned char).
         */
        template<typename Target, typename Source>
        struct is_xchar_to_xchar
        {
            typedef autoboost::mpl::bool_<
                     sizeof(Source) == sizeof(Target) &&
                     sizeof(Source) == sizeof(char) &&
                     autoboost::detail::is_character<Target>::value &&
                     autoboost::detail::is_character<Source>::value
                > type;

            AUTOBOOST_STATIC_CONSTANT(bool, value = (
                type::value
            ));
        };

        template<typename Target, typename Source>
        struct is_char_array_to_stdstring
            : autoboost::false_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_char_array_to_stdstring< std::basic_string<CharT, Traits, Alloc>, CharT* >
            : autoboost::true_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_char_array_to_stdstring< std::basic_string<CharT, Traits, Alloc>, const CharT* >
            : autoboost::true_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_char_array_to_stdstring< autoboost::container::basic_string<CharT, Traits, Alloc>, CharT* >
            : autoboost::true_type
        {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_char_array_to_stdstring< autoboost::container::basic_string<CharT, Traits, Alloc>, const CharT* >
            : autoboost::true_type
        {};

        template <typename Target, typename Source>
        struct copy_converter_impl
        {
// MSVC fail to forward an array (DevDiv#555157 "SILENT BAD CODEGEN triggered by perfect forwarding",
// fixed in 2013 RTM).
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && (!defined(AUTOBOOST_MSVC) || AUTOBOOST_MSVC >= 1800)
            template <class T>
            static inline bool try_convert(T&& arg, Target& result) {
                result = static_cast<T&&>(arg); // eqaul to `result = std::forward<T>(arg);`
                return true;
            }
#else
            static inline bool try_convert(const Source& arg, Target& result) {
                result = arg;
                return true;
            }
#endif
        };
    }

    namespace conversion { namespace detail {

        template <typename Target, typename Source>
        inline bool try_lexical_convert(const Source& arg, Target& result)
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::array_to_pointer_decay<Source>::type src;

            typedef autoboost::mpl::bool_<
                autoboost::detail::is_xchar_to_xchar<Target, src >::value ||
                autoboost::detail::is_char_array_to_stdstring<Target, src >::value ||
                (
                     autoboost::is_same<Target, src >::value &&
                     autoboost::detail::is_stdstring<Target >::value
                ) ||
                (
                     autoboost::is_same<Target, src >::value &&
                     autoboost::detail::is_character<Target >::value
                )
            > shall_we_copy_t;

            typedef autoboost::detail::is_arithmetic_and_not_xchars<Target, src >
                shall_we_copy_with_dynamic_check_t;

            // We do evaluate second `if_` lazily to avoid unnecessary instantiations
            // of `shall_we_copy_with_dynamic_check_t` and improve compilation times.
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_c<
                shall_we_copy_t::value,
                autoboost::mpl::identity<autoboost::detail::copy_converter_impl<Target, src > >,
                autoboost::mpl::if_<
                     shall_we_copy_with_dynamic_check_t,
                     autoboost::detail::dynamic_num_converter_impl<Target, src >,
                     autoboost::detail::lexical_converter_impl<Target, src >
                >
            >::type caster_type_lazy;

            typedef AUTOBOOST_DEDUCED_TYPENAME caster_type_lazy::type caster_type;

            return caster_type::try_convert(arg, result);
        }

        template <typename Target, typename CharacterT>
        inline bool try_lexical_convert(const CharacterT* chars, std::size_t count, Target& result)
        {
            AUTOBOOST_STATIC_ASSERT_MSG(
                autoboost::detail::is_character<CharacterT>::value,
                "This overload of try_lexical_convert is meant to be used only with arrays of characters."
            );
            return ::autoboost::conversion::detail::try_lexical_convert(
                ::autoboost::iterator_range<const CharacterT*>(chars, chars + count), result
            );
        }

    }} // namespace conversion::detail

    namespace conversion {
        // ADL barrier
        using ::autoboost::conversion::detail::try_lexical_convert;
    }

} // namespace autoboost

#if defined(__clang__) || (defined(__GNUC__) && \
    !(defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && \
    (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
#pragma GCC diagnostic pop
#endif

#endif // AUTOBOOST_LEXICAL_CAST_TRY_LEXICAL_CONVERT_HPP

