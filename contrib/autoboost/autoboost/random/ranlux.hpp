/* autoboost random/ranlux.hpp header file
 *
 * Copyright Jens Maurer 2002
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 *
 * Revision history
 *  2001-02-18  created
 */

#ifndef AUTOBOOST_RANDOM_RANLUX_HPP
#define AUTOBOOST_RANDOM_RANLUX_HPP

#include <autoboost/config.hpp>
#include <autoboost/random/subtract_with_carry.hpp>
#include <autoboost/random/discard_block.hpp>

namespace autoboost {

namespace random {

namespace detail {
/**
 * The ranlux family of generators are described in
 *
 *  @blockquote
 *  "A portable high-quality random number generator for lattice field theory
 *  calculations", M. Luescher, Computer Physics Communications, 79 (1994)
 *  pp 100-110.
 *  @endblockquote
 *
 * The levels are given in
 *
 *  @blockquote
 *  "RANLUX: A Fortran implementation of the high-quality
 *  pseudorandom number generator of Luescher", F. James,
 *  Computer Physics Communications 79 (1994) 111-114
 *  @endblockquote
 */
class ranlux_documentation {};
}

typedef subtract_with_carry_engine<uint32_t, 24, 10, 24> ranlux_base;
typedef subtract_with_carry_01_engine<float, 24, 10, 24> ranlux_base_01;
typedef subtract_with_carry_01_engine<double, 48, 10, 24> ranlux64_base_01;


/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux_base, 223, 24> ranlux3;
/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux_base, 389, 24> ranlux4;

/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux_base_01, 223, 24> ranlux3_01;
/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux_base_01, 389, 24> ranlux4_01;

/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux64_base_01, 223, 24> ranlux64_3_01;
/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux64_base_01, 389, 24> ranlux64_4_01;

#if !defined(AUTOBOOST_NO_INT64_T) && !defined(AUTOBOOST_NO_INTEGRAL_INT64_T)
typedef subtract_with_carry_engine<uint64_t, 48, 10, 24> ranlux64_base;
/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux64_base, 223, 24> ranlux64_3;
/** @copydoc autoboost::random::detail::ranlux_documentation */
typedef discard_block_engine<ranlux64_base, 389, 24> ranlux64_4;
#endif /* !AUTOBOOST_NO_INT64_T && !AUTOBOOST_NO_INTEGRAL_INT64_T */


typedef subtract_with_carry_engine<uint32_t, 24, 10, 24> ranlux24_base;
typedef subtract_with_carry_engine<uint64_t, 48, 5, 12> ranlux48_base;

typedef discard_block_engine<ranlux24_base, 223, 23> ranlux24;
#if !defined(AUTOBOOST_NO_INT64_T) && !defined(AUTOBOOST_NO_INTEGRAL_INT64_T)
typedef discard_block_engine<ranlux48_base, 389, 11> ranlux48;
#endif
}

using random::ranlux3;
using random::ranlux4;
using random::ranlux3_01;
using random::ranlux4_01;
using random::ranlux64_3_01;
using random::ranlux64_4_01;
#if !defined(AUTOBOOST_NO_INT64_T) && !defined(AUTOBOOST_NO_INTEGRAL_INT64_T)
using random::ranlux64_3;
using random::ranlux64_4;
#endif

} // namespace autoboost

#endif // AUTOBOOST_RANDOM_LINEAR_CONGRUENTIAL_HPP
