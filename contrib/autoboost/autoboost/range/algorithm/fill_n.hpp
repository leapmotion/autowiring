//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_FILL_N_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_FILL_N_HPP_INCLUDED

#include <autoboost/assert.hpp>
#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function fill_n
///
/// range-based version of the fill_n std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
/// \pre n <= std::distance(autoboost::begin(rng), autoboost::end(rng))
template< class ForwardRange, class Size, class Value >
inline ForwardRange& fill_n(ForwardRange& rng, Size n, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    AUTOBOOST_ASSERT( static_cast<Size>(std::distance(autoboost::begin(rng), autoboost::end(rng))) >= n );
    std::fill_n(autoboost::begin(rng), n, val);
    return rng;
}

/// \overload
template< class ForwardRange, class Size, class Value >
inline const ForwardRange& fill_n(const ForwardRange& rng, Size n, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    AUTOBOOST_ASSERT( static_cast<Size>(std::distance(autoboost::begin(rng), autoboost::end(rng))) >= n );
    std::fill_n(autoboost::begin(rng), n, val);
    return rng;
}

    } // namespace range
    using range::fill_n;
} // namespace autoboost

#endif // include guard
