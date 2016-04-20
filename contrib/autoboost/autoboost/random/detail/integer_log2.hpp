/* autoboost random/detail/integer_log2.hpp header file
 *
 * Copyright Steven Watanabe 2011
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 *
 */

#ifndef AUTOBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP
#define AUTOBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP

#include <autoboost/config.hpp>
#include <autoboost/limits.hpp>
#include <autoboost/pending/integer_log2.hpp>

namespace autoboost {
namespace random {
namespace detail {

#if !defined(AUTOBOOST_NO_CXX11_CONSTEXPR)
#define AUTOBOOST_RANDOM_DETAIL_CONSTEXPR constexpr
#elif defined(AUTOBOOST_MSVC)
#define AUTOBOOST_RANDOM_DETAIL_CONSTEXPR __forceinline
#elif defined(__GNUC__) && __GNUC__ >= 4
#define AUTOBOOST_RANDOM_DETAIL_CONSTEXPR inline __attribute__((__const__)) __attribute__((__always_inline__))
#else
#define AUTOBOOST_RANDOM_DETAIL_CONSTEXPR inline
#endif

template<int Shift>
struct integer_log2_impl
{
#if defined(AUTOBOOST_NO_CXX11_CONSTEXPR)
    template<class T>
    AUTOBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        int update = ((t >> Shift) != 0) * Shift;
        return integer_log2_impl<Shift / 2>::apply(t >> update, accum + update);
    }
#else
    template<class T>
    AUTOBOOST_RANDOM_DETAIL_CONSTEXPR static int apply2(T t, int accum, int update)
    {
        return integer_log2_impl<Shift / 2>::apply(t >> update, accum + update);
    }

    template<class T>
    AUTOBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        return apply2(t, accum, ((t >> Shift) != 0) * Shift);
    }
#endif
};

template<>
struct integer_log2_impl<1>
{
    template<class T>
    AUTOBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        return int(t >> 1) + accum;
    }
};

template<class T>
AUTOBOOST_RANDOM_DETAIL_CONSTEXPR int integer_log2(T t)
{
    return integer_log2_impl<
        ::autoboost::detail::max_pow2_less<
            ::std::numeric_limits<T>::digits, 4
        >::value
    >::apply(t, 0);
}

} // namespace detail
} // namespace random
} // namespace autoboost

#endif // AUTOBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP
