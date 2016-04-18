//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
namespace autoboost { namespace numeric {


    template <>
    struct numeric_cast_traits
        <
            char
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            char
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            signed char
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            signed char
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned char
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned char
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            short
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            short
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned short
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned short
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            int
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            int
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned int
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned int
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            long
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            long
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned long
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            unsigned long
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            float
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            float
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            double
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            double
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            long double
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            long double
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            autoboost::long_long_type
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            autoboost::long_long_type
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            autoboost::ulong_long_type
          , autoboost::long_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::long_long_type> rounding_policy;
    };

    template <>
    struct numeric_cast_traits
        <
            autoboost::ulong_long_type
          , autoboost::ulong_long_type
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<autoboost::ulong_long_type> rounding_policy;
    };
}}
