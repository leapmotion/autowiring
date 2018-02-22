/* autoboost random/generate_canonical.hpp header file
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

#ifndef AUTOBOOST_RANDOM_GENERATE_CANONICAL_HPP
#define AUTOBOOST_RANDOM_GENERATE_CANONICAL_HPP

#include <algorithm>
#include <autoboost/assert.hpp>
#include <autoboost/config/no_tr1/cmath.hpp>
#include <autoboost/limits.hpp>
#include <autoboost/type_traits/is_integral.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/random/detail/signed_unsigned_tools.hpp>
#include <autoboost/random/detail/generator_bits.hpp>

namespace autoboost {
namespace random {

namespace detail {

template<class RealType, std::size_t bits, class URNG>
RealType generate_canonical_impl(URNG& g, autoboost::mpl::true_ /*is_integral*/)
{
    using std::pow;
    typedef typename URNG::result_type base_result;
    std::size_t digits = std::numeric_limits<RealType>::digits;
    RealType R = RealType((g.max)()) - RealType((g.min)()) + 1;
    RealType mult = R;
    RealType limit =
        pow(RealType(2),
            RealType((std::min)(static_cast<std::size_t>(bits), digits)));
    RealType S = RealType(detail::subtract<base_result>()(g(), (g.min)()));
    while(mult < limit) {
        RealType inc = RealType(detail::subtract<base_result>()(g(), (g.min)()));
        S += inc * mult;
        mult *= R;
    }
    return S / mult;
}

template<class RealType, std::size_t bits, class URNG>
RealType generate_canonical_impl(URNG& g, autoboost::mpl::false_ /*is_integral*/)
{
    using std::pow;
    using std::floor;
    AUTOBOOST_ASSERT((g.min)() == 0);
    AUTOBOOST_ASSERT((g.max)() == 1);
    std::size_t digits = std::numeric_limits<RealType>::digits;
    std::size_t engine_bits = detail::generator_bits<URNG>::value();
    std::size_t b = (std::min)(bits, digits);
    RealType R = pow(RealType(2), RealType(engine_bits));
    RealType mult = R;
    RealType limit = pow(RealType(2), RealType(b));
    RealType S = RealType(g() - (g.min)());
    while(mult < limit) {
        RealType inc(floor((RealType(g()) - RealType((g.min)())) * R));
        S += inc * mult;
        mult *= R;
    }
    return S / mult;
}

}

/**
 * Returns a value uniformly distributed in the range [0, 1)
 * with at least @c bits random bits.
 */
template<class RealType, std::size_t bits, class URNG>
RealType generate_canonical(URNG& g)
{
    RealType result = detail::generate_canonical_impl<RealType, bits>(
        g, autoboost::random::traits::is_integral<typename URNG::result_type>());
    AUTOBOOST_ASSERT(result >= 0);
    AUTOBOOST_ASSERT(result <= 1);
    if(result == 1) {
        result -= std::numeric_limits<RealType>::epsilon() / 2;
        AUTOBOOST_ASSERT(result != 1);
    }
    return result;
}

} // namespace random
} // namespace autoboost

#endif // AUTOBOOST_RANDOM_GENERATE_CANONICAL_HPP
