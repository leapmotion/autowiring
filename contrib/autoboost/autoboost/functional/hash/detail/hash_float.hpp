
// Copyright 2005-2012 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(AUTOBOOST_FUNCTIONAL_HASH_DETAIL_HASH_FLOAT_HEADER)
#define AUTOBOOST_FUNCTIONAL_HASH_DETAIL_HASH_FLOAT_HEADER

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <autoboost/functional/hash/detail/float_functions.hpp>
#include <autoboost/functional/hash/detail/limits.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/integer/static_log2.hpp>
#include <autoboost/cstdint.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/limits.hpp>
#include <cstring>

#if defined(AUTOBOOST_MSVC)
#pragma warning(push)
#if AUTOBOOST_MSVC >= 1400
#pragma warning(disable:6294) // Ill-defined for-loop: initial condition does
                              // not satisfy test. Loop body not executed
#endif
#endif

// Can we use fpclassify?

// STLport
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define AUTOBOOST_HASH_USE_FPCLASSIFY 0

// GNU libstdc++ 3
#elif defined(__GLIBCPP__) || defined(__GLIBCXX__)
#  if (defined(__USE_ISOC99) || defined(_GLIBCXX_USE_C99_MATH)) && \
      !(defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__))
#    define AUTOBOOST_HASH_USE_FPCLASSIFY 1
#  else
#    define AUTOBOOST_HASH_USE_FPCLASSIFY 0
#  endif

// Everything else
#else
#  define AUTOBOOST_HASH_USE_FPCLASSIFY 0
#endif

namespace autoboost
{
    namespace hash_detail
    {
        inline void hash_float_combine(std::size_t& seed, std::size_t value)
        {
            seed ^= value + (seed<<6) + (seed>>2);
        }

        ////////////////////////////////////////////////////////////////////////
        // Binary hash function
        //
        // Only used for floats with known iec559 floats, and certain values in
        // numeric_limits

        inline std::size_t hash_binary(char* ptr, std::size_t length)
        {
            std::size_t seed = 0;

            if (length >= sizeof(std::size_t)) {
                std::memcpy(&seed, ptr, sizeof(std::size_t));
                length -= sizeof(std::size_t);
                ptr += sizeof(std::size_t);

                while(length >= sizeof(std::size_t)) {
                    std::size_t buffer = 0;
                    std::memcpy(&buffer, ptr, sizeof(std::size_t));
                    hash_float_combine(seed, buffer);
                    length -= sizeof(std::size_t);
                    ptr += sizeof(std::size_t);
                }
            }

            if (length > 0) {
                std::size_t buffer = 0;
                std::memcpy(&buffer, ptr, length);
                hash_float_combine(seed, buffer);
            }

            return seed;
        }

        template <typename Float, unsigned digits, unsigned max_exponent>
        struct enable_binary_hash
        {
            AUTOBOOST_STATIC_CONSTANT(bool, value =
                std::numeric_limits<Float>::is_iec559 &&
                std::numeric_limits<Float>::digits == digits &&
                std::numeric_limits<Float>::radix == 2 &&
                std::numeric_limits<Float>::max_exponent == max_exponent);
        };

        template <typename Float>
        inline std::size_t float_hash_impl(Float v,
            AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if_c<
                enable_binary_hash<Float, 24, 128>::value,
                std::size_t>::type)
        {
            return hash_binary((char*) &v, 4);
        }


        template <typename Float>
        inline std::size_t float_hash_impl(Float v,
            AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if_c<
                enable_binary_hash<Float, 53, 1024>::value,
                std::size_t>::type)
        {
            return hash_binary((char*) &v, 8);
        }

        template <typename Float>
        inline std::size_t float_hash_impl(Float v,
            AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if_c<
                enable_binary_hash<Float, 64, 16384>::value,
                std::size_t>::type)
        {
            return hash_binary((char*) &v, 10);
        }

        template <typename Float>
        inline std::size_t float_hash_impl(Float v,
            AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if_c<
                enable_binary_hash<Float, 113, 16384>::value,
                std::size_t>::type)
        {
            return hash_binary((char*) &v, 16);
        }

        ////////////////////////////////////////////////////////////////////////
        // Portable hash function
        //
        // Used as a fallback when the binary hash function isn't supported.

        template <class T>
        inline std::size_t float_hash_impl2(T v)
        {
            autoboost::hash_detail::call_frexp<T> frexp;
            autoboost::hash_detail::call_ldexp<T> ldexp;

            int exp = 0;

            v = frexp(v, &exp);

            // A postive value is easier to hash, so combine the
            // sign with the exponent and use the absolute value.
            if(v < 0) {
                v = -v;
                exp += limits<T>::max_exponent -
                    limits<T>::min_exponent;
            }

            v = ldexp(v, limits<std::size_t>::digits);
            std::size_t seed = static_cast<std::size_t>(v);
            v -= static_cast<T>(seed);

            // ceiling(digits(T) * log2(radix(T))/ digits(size_t)) - 1;
            std::size_t const length
                = (limits<T>::digits *
                        autoboost::static_log2<limits<T>::radix>::value
                        + limits<std::size_t>::digits - 1)
                / limits<std::size_t>::digits;

            for(std::size_t i = 0; i != length; ++i)
            {
                v = ldexp(v, limits<std::size_t>::digits);
                std::size_t part = static_cast<std::size_t>(v);
                v -= static_cast<T>(part);
                hash_float_combine(seed, part);
            }

            hash_float_combine(seed, static_cast<std::size_t>(exp));

            return seed;
        }

#if !defined(AUTOBOOST_HASH_DETAIL_TEST_WITHOUT_GENERIC)
        template <class T>
        inline std::size_t float_hash_impl(T v, ...)
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME select_hash_type<T>::type type;
            return float_hash_impl2(static_cast<type>(v));
        }
#endif
    }
}

#if AUTOBOOST_HASH_USE_FPCLASSIFY

#include <autoboost/config/no_tr1/cmath.hpp>

namespace autoboost
{
    namespace hash_detail
    {
        template <class T>
        inline std::size_t float_hash_value(T v)
        {
#if defined(fpclassify)
            switch (fpclassify(v))
#elif AUTOBOOST_HASH_CONFORMANT_FLOATS
            switch (std::fpclassify(v))
#else
            using namespace std;
            switch (fpclassify(v))
#endif
            {
            case FP_ZERO:
                return 0;
            case FP_INFINITE:
                return (std::size_t)(v > 0 ? -1 : -2);
            case FP_NAN:
                return (std::size_t)(-3);
            case FP_NORMAL:
            case FP_SUBNORMAL:
                return float_hash_impl(v, 0);
            default:
                AUTOBOOST_ASSERT(0);
                return 0;
            }
        }
    }
}

#else // !AUTOBOOST_HASH_USE_FPCLASSIFY

namespace autoboost
{
    namespace hash_detail
    {
        template <class T>
        inline bool is_zero(T v)
        {
#if !defined(__GNUC__)
            return v == 0;
#else
            // GCC's '-Wfloat-equal' will complain about comparing
            // v to 0, but because it disables warnings for system
            // headers it won't complain if you use std::equal_to to
            // compare with 0. Resulting in this silliness:
            return std::equal_to<T>()(v, 0);
#endif
        }

        template <class T>
        inline std::size_t float_hash_value(T v)
        {
            return autoboost::hash_detail::is_zero(v) ? 0 : float_hash_impl(v, 0);
        }
    }
}

#endif // AUTOBOOST_HASH_USE_FPCLASSIFY

#undef AUTOBOOST_HASH_USE_FPCLASSIFY

#if defined(AUTOBOOST_MSVC)
#pragma warning(pop)
#endif

#endif
